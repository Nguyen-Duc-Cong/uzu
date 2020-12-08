#include "generator.h"
#include "constances.h"
#include "graph.h"
#include "heap.h"
#include "macros.h"
#include "map.h"
#include "priority-queue.h"
#include "ptrarray.h"
#include "toolbox.h"
#include "types.h"
#include <SDL2/SDL_assert.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROOMS 100

typedef struct Room
{
  SDL_Rect rect;
} Room;

extern SDL_Renderer* g_renderer;

static Room      _rooms[MAX_ROOMS];
static int       _room_cnt;
static Graph*    _graph;
static PtrArray* _tree;
static int       _param_width;
static int       _param_height;
static int       _param_max_room_size;
static int       _param_min_room_size;

/*helper functions*/

static void astar(int x1, int y1, int x2, int y2);

/*end of helpers*/

INLINE int rand_range(int a, int b)
{
  if (a == b)
    return a;
  return rand() % (b - a) + a;
}

static void place(int x, int y, int w, int h)
{
  if (_room_cnt >= MAX_ROOMS)
    return;
  if (w < _param_max_room_size || h <= _param_max_room_size)
  {
    int rw = w >= _param_min_room_size ? rand_range(_param_min_room_size, w) : w;
    int rh = h >= _param_min_room_size ? rand_range(_param_min_room_size, h) : h;
    int rx = rand_range(x, x + w - rw);
    int ry = rand_range(y, y + h - rh);
    _rooms[_room_cnt++].rect = (SDL_Rect){rx, ry, rw, rh};
  }
  else
  {
    if (w > h)
    {
      int r = rand_range(4, 6);

      int w1 = w * r / 10;
      int w2 = w - w1;

      place(x, y, w1, h);
      place(x + w1, y, w2, h);
    }
    else
    {
      int r = rand_range(4, 6);
      int h1 = h * r / 10;
      int h2 = h - h1;
      place(x, y, w, h1);
      place(x, y + h1, w, h2);
    }
  }
}

static void triangulation()
{
  graph_delete(_graph);
  _graph = NULL;
  Point     points[_room_cnt];
  pointer_t user_data_array[_room_cnt];
  for (int i = 0; i < _room_cnt; ++i)
  {
    points[i].x = _rooms[i].rect.x + _rooms[i].rect.w / 2.f;
    points[i].y = _rooms[i].rect.y + _rooms[i].rect.h / 2.f;
    user_data_array[i] = &_rooms[i];
  }
  _graph = graph_new_triangulation(points, user_data_array, _room_cnt);
}

static void minimum_spanning_tree()
{
  ptr_array_delete(_tree);
  _tree = graph_minimum_spanning_tree(_graph);
}

static void build_room(const Room* room)
{
  int  left, right, top, bottom, col_cnt, row_cnt;
  int* floor_layer;

  left = room->rect.x;
  right = left + room->rect.w - 1;
  top = room->rect.y;
  bottom = top + room->rect.h - 1;
  floor_layer = map_get_layer(MAP_LAYER_FLOOR);

  map_get_size(&col_cnt, &row_cnt);

  for (int y = top; y <= bottom; ++y)
    for (int x = left; x <= right; ++x)
      floor_layer[y * col_cnt + x] = 1;
}

INLINE float distance(int x1, int y1, int x2, int y2)
{
  float a = x1 - x2;
  float b = y1 - y2;
  return SDL_sqrtf(a * a + b * b);
}

INLINE float heuristic(int x1, int y1, int x2, int y2) { return ABS(x1 - x2) + ABS(y1 - y2); }

typedef struct GirdNode
{
  int x, y;
} GirdNode;

typedef struct
{
  GirdNode node;
  float    priority;
} PrioritizedGirdNode;

static int compare_node_priority(const PrioritizedGirdNode* a, const PrioritizedGirdNode* b)
{
  return a->priority - b->priority;
}

static PrioritizedGirdNode* new_gird_node(int x, int y, float priority)
{
  PrioritizedGirdNode* p = malloc(sizeof(PrioritizedGirdNode));
  p->node = (GirdNode){x, y};
  p->priority = priority;
  return p;
}

static void get_neighbors(int x, int y, int* neighbor_x, int* neighbor_y, int* cnt)
{

  static const int DX[4] = {0, 0, -1, 1};
  static const int DY[4] = {-1, 1, 0, 0};

  *cnt = 0;
  int nx;
  int ny;
  for (int i = 0; i < 4; ++i)
  {
    nx = x + DX[i];
    ny = y + DY[i];
    if (nx < 0 || nx >= _param_width)
      continue;
    if (ny < 0 || ny >= _param_height)
      continue;
    neighbor_x[*cnt] = nx;
    neighbor_y[*cnt] = ny;
    (*cnt)++;
  }
}

static void retrieve(const GirdNode* came_from, int dst_x, int dst_y)
{
  int  x, y, i;
  int* floor_layer;

  x = dst_x;
  y = dst_y;
  floor_layer = map_get_layer(MAP_LAYER_FLOOR);

  while (x != -1 && y != -1)
  {
    i = y * _param_width + x;
    floor_layer[i] = 16;
    x = came_from[i].x;
    y = came_from[i].y;
  }
}

static void astar(int x1, int y1, int x2, int y2)
{
  bool*                visited = NULL;
  GirdNode*            came_from = NULL;
  PriorityQueue*       queue = NULL; /* PriorityQueue<PrioritizedGirdNode> */
  PrioritizedGirdNode* curr;
  int                  curr_x, curr_y, index;
  int                  next_x[4] = {}, next_y[4] = {};
  int                  neighbor_cnt, tile_cnt;
  float                priority;
  bool                 found = false;

  tile_cnt = _param_width * _param_height;
  visited = calloc(tile_cnt, sizeof(bool));
  came_from = calloc(tile_cnt, sizeof(GirdNode));
  queue = priority_queue_new((compare_fn_t)compare_node_priority, (destroy_fn_t)free);

  for (int i = 0; i < tile_cnt; ++i)
    came_from[i] = (GirdNode){-1, -1};

  visited[y1 * _param_width + x1] = true;
  priority_queue_offer(queue, new_gird_node(x1, y1, 0));

  while (HEAP(queue)->cnt > 0)
  {
    curr = priority_queue_poll(queue);
    curr_x = curr->node.x;
    curr_y = curr->node.y;

    if (curr_x == x2 && curr_y == y2)
    {
      found = true;
      break;
    }

    get_neighbors(curr_x, curr_y, next_x, next_y, &neighbor_cnt);

    for (int i = 0; i < neighbor_cnt; ++i)
    {
      index = next_y[i] * _param_width + next_x[i];
      if (!visited[index])
      {
        visited[index] = true;
        came_from[index] = (GirdNode){curr_x, curr_y};
        priority = heuristic(x2, y2, next_x[i], next_y[i]);
        priority_queue_offer(queue, new_gird_node(next_x[i], next_y[i], priority));
      }
    }
    free(curr);
  }

  if (found)
  {
    retrieve(came_from, x2, y2);
  }

  free(visited);
  free(came_from);
  priority_queue_delete(queue);
}

static void connect_room(const Room* room1, const Room* room2)
{
  int cx1 = room1->rect.x + room1->rect.w / 2;
  int cy1 = room1->rect.y + room1->rect.h / 2;
  int cx2 = room2->rect.x + room2->rect.w / 2;
  int cy2 = room2->rect.y + room2->rect.h / 2;

  astar(cx1, cy1, cx2, cy2);
}

static void build_map_from_graph()
{
  int  cnt = _room_cnt;
  int* floor_layer;

  map_set_size(_param_width, _param_height);

  floor_layer = map_get_layer(MAP_LAYER_FLOOR);

  memset(floor_layer, 0, sizeof(int) * _param_width * _param_height);
  for (int i = 0; i < cnt; ++i)
  {
    build_room(&_rooms[i]);
  }

  cnt = _tree->cnt;
  for (int i = 0; i < cnt; ++i)
  {
    Edge* e = _tree->storage[i];
    Room* room1 = e->a->data;
    Room* room2 = e->b->data;
    connect_room(room1, room2);
  }
}

void set_generator_param(int pram, int value)
{
  switch (pram)
  {
  case GENERATOR_PARAM_WIDTH:
    _param_width = value;
    break;
  case GENERATOR_PARAM_HEIGHT:
    _param_height = value;
    break;
  case GENERATOR_PARAM_MAX_ROOM_SIZE:
    _param_max_room_size = value;
    break;
  case GENERATOR_PARAM_MIN_ROOM_SIZE:
    _param_min_room_size = value;
    break;
  }
}

void generate_new_dungeon()
{
  _room_cnt = 0;
  int s = TILE_SIZE;
  place(0, 0, _param_width, _param_height);
  triangulation();
  minimum_spanning_tree();
  build_map_from_graph();
}

#ifdef DEBUG

void draw_rooms(int s)
{
  SDL_Rect r;
  SDL_SetRenderDrawColor(g_renderer, 0xff, 0xff, 0xff, 0xff);
  for (int i = 0; i < _room_cnt; ++i)
  {
    r.x = _rooms[i].rect.x * s;
    r.y = _rooms[i].rect.y * s;
    r.w = _rooms[i].rect.w * s;
    r.h = _rooms[i].rect.h * s;
    SDL_RenderDrawRect(g_renderer, &r);
  }
}

void draw_graph(int s)
{
  SDL_SetRenderDrawColor(g_renderer, 0x00, 0xff, 0xff, 0x00);
  for (int i = 0; i < _graph->edges->cnt; ++i)
  {
    Edge* e = _graph->edges->storage[i];
    SDL_RenderDrawLine(g_renderer, e->a->p.x * s, e->a->p.y * s, e->b->p.x * s, e->b->p.y * s);
  }
}

void draw_tree(int s)
{
  SDL_SetRenderDrawColor(g_renderer, 0xff, 0x00, 0x00, 0xff);
  for (int i = 0; i < _tree->cnt; ++i)
  {
    Edge* e = _tree->storage[i];
    SDL_RenderDrawLine(g_renderer, e->a->p.x * s, e->a->p.y * s, e->b->p.x * s, e->b->p.y * s);
  }
}

#endif
