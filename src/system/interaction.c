#include "system/game_logic.h"

#include "entity_utils.h"
#include "system/collision_sys.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "ui_list.h"
#include "system/event_messaging_sys.h"

#define INTERACTABLE_DISTANCE TILE_SIZE * 4
#define POINTER_DOWN_WIDTH 6
#define POINTER_DOWN_HEIGHT 3

extern ecs_entity_t  g_curr_iteractable_entity;
extern Ecs*          g_ecs;
extern RECT          g_viewport;
extern SDL_Renderer* g_renderer;

void interactable_rendering_system()
{
  if (g_curr_iteractable_entity != ECS_NULL_ENT)
  {
    Vec2 position = get_entity_position(g_ecs, g_curr_iteractable_entity);

    SDL_RenderCopy(g_renderer,
                   get_texture(TEX_PONITER_DOWN),
                   NULL,
                   &(RECT){
                       position.x - g_viewport.x - POINTER_DOWN_WIDTH / 2,
                       position.y - 2 * TILE_SIZE - g_viewport.y,
                       POINTER_DOWN_WIDTH,
                       POINTER_DOWN_HEIGHT,
                   });
  }
}
