#include "inventory.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "game_scene.h"
#include "item.h"
#include "resources.h"
#include "ui_helper.h"

#define INV_CELL_SIZE 16
#define INV_CELL_GAP 1
#define INV_X 16
#define INV_Y 32
#define INV_COL 5
#define INV_ROW 5
#define INV_MAX_SLOT 25
extern SDL_Renderer* g_renderer;

static Item _items[NUM_ITEM_CATEGORIES][INV_MAX_SLOT];
static u32  _cnt[NUM_ITEM_CATEGORIES];
static s32  _curr_col, _curr_row;

static BOOL _visible;

static const RECT description_rect = { 100, INV_Y, 100, 25 };

static void process_key_input(void);

static int find_item(Item* items, u32 cnt, ItemTypeId type_id)
{
  for (u32 i = 0; i < cnt; ++i)
    if (items[i].type_id == type_id)
      return i;
  return -1;
}

static void play_sound(SfxId id)
{
  Mix_PlayChannel(-1, get_sfx(id), 0);
}

static void process_key_input(void)
{
  if (key_just_pressed(KEY_LEFT))
  {
    _curr_col = max(0, _curr_col - 1);
    play_sound(SFX_BUTTON);
  }
  if (key_just_pressed(KEY_RIGHT))
  {
    _curr_col = min(INV_COL - 1, _curr_col + 1);
    play_sound(SFX_BUTTON);
  }
  if (key_just_pressed(KEY_DOWN))
  {
    _curr_row = min(INV_ROW - 1, _curr_row + 1);
    play_sound(SFX_BUTTON);
  }
  if (key_just_pressed(KEY_UP))
  {
    _curr_row = max(0, _curr_row - 1);
    play_sound(SFX_BUTTON);
  }
  if (key_just_pressed(KEY_B))
  {
    inventory_close();
    play_sound(SFX_INTERACTION);
  }
}

BOOL add_to_inv(ItemTypeId type_id)
{
  ItemCategory cat   = g_item_types[type_id].category;
  Item*        items = _items[cat];
  u32*         cnt   = &_cnt[cat];
  int          idx   = find_item(items, *cnt, type_id);

  if (idx == -1)
  {
    if (*cnt < INV_MAX_SLOT)
    {
      items[(*cnt)++] = (Item){ .type_id = type_id, .cnt = 1 };
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    if (items[idx].cnt < ITEM_MAX_STACK)
    {
      items[idx].cnt++;
      return TRUE;
    }
    return FALSE;
  }
  return FALSE;
}

void inventory_display()
{
  game_scene_pause();
  _visible = TRUE;
  keybroad_push_state(process_key_input);
}

void inventory_close()
{
  game_scene_resume();
  _visible = FALSE;
  keybroad_pop_state();
}

void inventory_draw()
{
  if (!_visible)
    return;

  RECT cell_rect;
  int  idx;

  Item*  items = _items[ITEM_CATEGORY_CONSUMABLE];
  Sprite sprite;

  for (int i = 0; i < INV_ROW; ++i)
  {
    for (int j = 0; j < INV_COL; ++j)
    {
      cell_rect = (RECT){
        j * INV_CELL_SIZE + INV_CELL_GAP + INV_X,
        i * INV_CELL_SIZE + INV_CELL_GAP + INV_Y,
        INV_CELL_SIZE,
        INV_CELL_SIZE,
      };
      draw_box_w_border(&cell_rect,
                        UI_COLOR_BG,
                        i == _curr_row && j == _curr_col ? UI_COLOR_BORDER_SELECT
                                                         : UI_COLOR_BORDER);

      idx    = i * INV_COL + j;
      sprite = g_item_types[items[idx].type_id].sprite;

      if (items[idx].cnt > 0)
      {
        SDL_RenderCopy(g_renderer, sprite.tex, &sprite.rect, &cell_rect);
        FC_DrawColor(get_font(FONT_DAMAGE_INDICATOR),
                     g_renderer,
                     cell_rect.x + 3,
                     cell_rect.y + 3,
                     (COLOR){ 232, 39, 194, 255 },
                     "%d",
                     items[idx].cnt);
      }
    }
  }

  SDL_SetRenderDrawColor(g_renderer, 0xff, 0xff, 0xff, 0xff);
  SDL_RenderDrawRect(g_renderer,
                     &(RECT){
                         INV_X,
                         INV_Y,
                         INV_COL * (INV_CELL_SIZE) + 1,
                         INV_ROW * (INV_CELL_SIZE) + 1,
                     });

  idx = _curr_col + _curr_row * INV_COL;

  draw_box_w_border(&description_rect, UI_COLOR_BG, UI_COLOR_BORDER);
  if (items[idx].cnt > 0)
  {
    FC_DrawBoxColor(get_font(FONT_DAMAGE_INDICATOR),
                    g_renderer,
                    (RECT){
                        description_rect.x + 3,
                        description_rect.y + 2,
                        description_rect.w - 4,
                        description_rect.h - 5,
                    },
                    UI_COLOR_TEXT,
                    g_item_types[items[idx].type_id].description);
  }
}
