#include "behaviour_tree.h"
#include "SDL_image.h"
#include "toolbox/toolbox.h"
#include "components.h"
#include "constances.h"
#include "dungeon.h"
#include "entity_factory.h"
#include "map.h"
#include "resources.h"
#include "scene.h"
#include "ecs/ecs.h"
#include "engine/engine.h"
#include "engine/keyboard.h"
#include "session.h"
#include <stdio.h>
#include <stdlib.h>

SDL_Rect g_viewport;

static const Scene* _curr_scr = NULL;

extern SDL_Renderer* g_renderer;
extern SDL_Window*   g_window;

#define SHOW_FPS 1

#if SHOW_FPS
static FONT* _fps_font;
#endif

static void on_game_quit();
static BOOL on_game_init();
static void on_game_fini();
static void on_game_quit();
static void on_game_loop();
static void on_event(const SDL_Event* e);

static BOOL on_game_init()
{
  SDL_RenderSetScale(g_renderer, SCL_X, SCL_Y);

  srand(SDL_GetTicks());

  g_viewport = (SDL_Rect){ 0, 0, WIN_WIDTH, WIN_HEIGHT };

  if (!resources_load())
  {
    ERROR("load resources failed\n");
    return FALSE;
  }

  set_scene(&g_main_menu);

#if SHOW_FPS
  _fps_font = get_font(FONT_DAMAGE_INDICATOR);
#endif

  return TRUE;
}

static void on_game_fini()
{
  if (_curr_scr != NULL)
    _curr_scr->on_unload();
  resources_unload();
}

static void on_game_loop()
{
  u32 start = SDL_GetTicks();
  if (_curr_scr != NULL)
    _curr_scr->on_update();
  FC_Draw(_fps_font, g_renderer, 0, 0, "%dms", SDL_GetTicks() - start);
}

static void on_event(const SDL_Event* e)
{
  if (e->type == SDL_QUIT)
  {
    on_game_quit();
  }
  else if (_curr_scr != NULL)
  {
    _curr_scr->on_event(e);
  }
}

static void on_game_quit()
{
  engine_stop();
}

void set_scene(const Scene* newscr)
{
  if (_curr_scr == newscr)
    return;
  if (_curr_scr != NULL)
    _curr_scr->on_unload();
  if (newscr != NULL)
    newscr->on_load();
  _curr_scr = newscr;
}

static GameDelegate delegate = {
  .init  = on_game_init,
  .fini  = on_game_fini,
  .loop  = on_game_loop,
  .event = on_event,
};

static GameSetting setting = {
  .frame_rate    = 60,
  .window_title  = "dungeon II",
  .window_width  = WIN_WIDTH * SCL_X,
  .window_height = WIN_HEIGHT * SCL_Y,
};

int main()
{
  engine_run(&delegate, &setting);
  return 0;
}
