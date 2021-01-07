#include "ui_quality.h"
#include "SDL.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "ui_helper.h"
extern SDL_Renderer* g_renderer;

static char* _title;
static BOOL  _visible;
static u32   _val;
static u32   _max_val;
static u32   _min_val;

static Callback _callbacks[UI_QUALITY_NUM_EVENTS];

#define UI_QUALITY_X 50
#define UI_QUALITY_Y 50

static const RECT BOX_RECT     = { UI_QUALITY_X, UI_QUALITY_Y, 50, 30 };
static const RECT TILE_RECT    = { UI_QUALITY_X + 3, UI_QUALITY_Y + 3, 44, 24 };
static const RECT QUALITY_RECT = { UI_QUALITY_X, UI_QUALITY_Y + 12, 44, 24 };

static void process_key_input()
{
  if (key_just_pressed(KEY_A))
  {
    keybroad_pop_state();
    _visible = FALSE;
    if (_callbacks[UI_QUALITY_ON_SELECTED].func != NULL)
      INVOKE_CALLBACK(_callbacks[UI_QUALITY_ON_SELECTED], void, _val);
    return;
  }

  if (key_just_pressed(KEY_B))
  {

    keybroad_pop_state();
    _visible = FALSE;
    return;
  }

  if (key_just_pressed(KEY_RIGHT))
  {
    if (_val < _max_val)
    {
      ++_val;
    }
    else
    {
      // TODO: play sound
    }
  }

  if (key_just_pressed(KEY_LEFT))
  {
    if (_val > _min_val)
    {
      --_val;
    }
    else
    {
      // TODO: play sound
    }
  }
}

void ui_quality_display(const char* title, u32 initial_val, u32 min_val, u32 max_val)
{
  _visible = TRUE;
  _title   = strdup(title);
  _val     = initial_val;
  _max_val = max_val;
  _min_val = min_val;
  keybroad_push_state(process_key_input);
}

void ui_quality_draw()
{
  if (!_visible)
    return;

  FONT*     font   = get_font(FONT_DAMAGE_INDICATOR);
  FC_Effect effect = { .scale = { 1.f, 1.f }, .alignment = FC_ALIGN_CENTER, .color = COLOR_WHITE };

  draw_box_w_border(&BOX_RECT, COLOR_BLACK, COLOR_WHITE);

  FC_DrawBoxColor(font, g_renderer, TILE_RECT, COLOR_WHITE, _title);
  FC_DrawBoxEffect(font, g_renderer, QUALITY_RECT, effect, "< %u >", _val);
}

void ui_quality_hook(s32 event_id, Callback callback)
{
  _callbacks[event_id] = callback;
}
