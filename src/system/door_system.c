#include "door_system.h"
#include "event_messaging_sys.h"
#include <components.h>
#include <constances.h>

#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

//<-------------------event callbacks---------------------->//
static void on_command_selected(void* arg, const MSG_CommandSelected* event);
static void on_entity_collied_door(void* arg, const MSG_HitDoor* event);
//==========================================================//

void door_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, NULL, on_command_selected);
  ems_connect(MSG_HIT_DOOR, NULL, on_entity_collied_door);
}

static void on_command_selected(SDL_UNUSED void* arg, const MSG_CommandSelected* event)
{

  DoorInfo*     door_info;
  Visual*       visual;
  Interactable* interactable;

  if ((door_info = ecs_get(g_ecs, event->entity, DOOR_INFO)))
  {
    visual       = ecs_get(g_ecs, event->entity, VISUAL);
    interactable = ecs_get(g_ecs, event->entity, INTERACTABLE);
    if (strcmp(event->cmd, TEXT_COMMAND_OPEN) == 0)
    {
      visual->sprite.rect       = RECT_DOOR_OPEN;
      door_info->state          = DOOR_STATE_OPEN;
      interactable->commands[0] = TEXT_COMMAND_CLOSE;
    }
    else if (strcmp(event->cmd, TEXT_COMMAND_CLOSE) == 0)
    {
      visual->sprite.rect       = RECT_DOOR_CLOSE;
      door_info->state          = DOOR_STATE_CLOSE;
      interactable->commands[0] = TEXT_COMMAND_OPEN;
    }
  }
}

static void on_entity_collied_door(SDL_UNUSED void* arg, const MSG_HitDoor* event)
{

  DoorInfo*  door_info;
  HitBox *   eh, *dh;
  Transform *etx, *dtx;

  RECT er;
  RECT dr;

  door_info = ecs_get(g_ecs, event->door, DOOR_INFO);

  if (door_info->state == DOOR_STATE_OPEN)
    return;

  eh  = ecs_get(g_ecs, event->entity, HITBOX);
  dh  = ecs_get(g_ecs, event->door, HITBOX);
  etx = ecs_get(g_ecs, event->entity, TRANSFORM);
  dtx = ecs_get(g_ecs, event->door, TRANSFORM);

  er.x = etx->pos.x - eh->anchor.x;
  er.y = etx->pos.y - 5;
  er.w = eh->size.x;
  er.h = 5;

  dr.x = dtx->pos.x - dh->anchor.x;
  dr.y = dtx->pos.y - dh->anchor.y;
  dr.w = dh->size.x;
  dr.h = dh->size.y;
  if (SDL_HasIntersection(&er, &dr))
  {
    etx->pos = etx->prev_pos;
  }
}
