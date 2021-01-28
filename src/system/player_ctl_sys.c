#include "player_ctl_sys.h"
#include "collision_sys.h"
#include "constances.h"
#include "inventory.h"
#include "ui_list.h"

#include "event_messaging_sys.h"
#include <components.h>
#include <ecs/ecs.h>
#include <engine/keyboard.h>
#include <utils.h>

extern Ecs* g_ecs;

#define INTERACTABLE_DISTANCE (TILE_SIZE * 1)
#define POINTER_DOWN_WIDTH 6
#define POINTER_DOWN_HEIGHT 3
#define MOVE_SPEED 100.f

ecs_entity_t g_curr_iteractable_entity = ECS_NULL_ENT;

extern Ecs* g_ecs;

//<--------------------------------event callbacks----------------------------------->//

static void on_list_select(pointer_t arg, const char* item);

//<==================================================================================>//

static void on_list_select(SDL_UNUSED pointer_t arg, const char* item)
{
  ems_broadcast(MSG_COMANND_SELECTED,
                &(MSG_CommandSelected){
                    g_curr_iteractable_entity,
                    item,
                });
}

struct CBPlayerController_QueryInteracableEntitiesArgs
{
  Vec2         player_position;
  float        closest_distance;
  ecs_entity_t closest_entity;
};

static BOOL
__cb_query_interacable_entities(struct CBPlayerController_QueryInteracableEntitiesArgs* args,
                                ecs_entity_t                                            e)
{
  Vec2  entity_position = get_entity_position(g_ecs, e);
  float dist            = vec2_dist(args->player_position, entity_position);
  if (dist < args->closest_distance)
  {
    args->closest_distance = dist;
    args->closest_entity   = e;
  }
  return TRUE;
}

static void find_interacable_entity()
{

  if (g_curr_iteractable_entity == ECS_NULL_ENT)
  {
    Vec2 player_pos;
    RECT rect;

    struct CBPlayerController_QueryInteracableEntitiesArgs cbargs;

    player_pos = get_player_position(g_ecs);
    rect.x     = player_pos.x - INTERACTABLE_DISTANCE / 2;
    rect.y     = player_pos.y - INTERACTABLE_DISTANCE / 2;
    rect.w     = INTERACTABLE_DISTANCE;
    rect.h     = INTERACTABLE_DISTANCE;

    cbargs.player_position  = player_pos;
    cbargs.closest_entity   = ECS_NULL_ENT;
    cbargs.closest_distance = 9999.f;

    collision_box_query(&rect,
                        MASK_INTERACABLE_ENTITY,
                        CALLBACK_1(&cbargs, __cb_query_interacable_entities));
    g_curr_iteractable_entity = cbargs.closest_entity;
  }
  else
  {
    Vec2 player_pos, ientity_pos;

    player_pos  = get_player_position(g_ecs);
    ientity_pos = get_entity_position(g_ecs, g_curr_iteractable_entity);

    if (vec2_dist(player_pos, ientity_pos) > INTERACTABLE_DISTANCE)
    {
      g_curr_iteractable_entity = ECS_NULL_ENT;
    }
  }
}

static void begin_interact(ecs_entity_t entity)
{
  Interactable* interactable;
  INFO("begin interact with e" ECS_ENT_FMT_PATTERN "\n", ECS_ENT_FMT_VARS(entity));

  ems_broadcast(MSG_BEGIN_INTERACTION, &(MSG_BeginInteraction){ entity });

  interactable = ecs_get(g_ecs, entity, INTERACTABLE);

  ui_list_display((const char**)interactable->commands, interactable->num_commands);
  ui_list_set_pos(120, 120);
  ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_select));
}

void player_controller_system_update()
{

  ecs_entity_t player;
  Controller*  controller;
  Motion*      motion;

  find_interacable_entity();

  if ((player = get_player(g_ecs)) == ECS_NULL_ENT)
    return;

  controller = ecs_get(g_ecs, player, CONTROLLER);
  motion     = ecs_get(g_ecs, player, MOTION);

  if (!ecs_has(g_ecs, player, INVULNERABLE))
  {

    motion->vel = VEC2_ZERO;
    if (key_pressed(KEY_UP))
    {
      motion->vel.y += -MOVE_SPEED;
    }

    if (key_pressed(KEY_DOWN))
    {
      motion->vel.y += MOVE_SPEED;
    }

    if (key_pressed(KEY_LEFT))
    {
      motion->vel.x += -MOVE_SPEED;
    }

    if (key_pressed(KEY_RIGHT))

    {
      motion->vel.x += MOVE_SPEED;
    }

    if (key_just_pressed(KEY_SELECT))
    {
      inventory_display();
      return;
    }

    if (g_curr_iteractable_entity == ECS_NULL_ENT)
    {
      if (key_just_pressed(KEY_A))
      {
        controller->action = CHARACTER_ACTION_REGULAR_ATK;
      }

      else if (key_just_pressed(KEY_B))
      {
        controller->action = CHARACTER_ACTION_SPECIAL_ATK;
      }
    }
    else
    {

      if (key_just_pressed(KEY_A))
        begin_interact(g_curr_iteractable_entity);
    }
  }
}
