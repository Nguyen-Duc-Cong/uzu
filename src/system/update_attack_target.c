#include "components.h"
#include "ecs/ecs.h"
#include "entity_utils.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

static void on_entity_died(SDL_UNUSED void* arg, MSG_EntityDied* event)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  AttackTarget* attack_target;
  ecs_raw(g_ecs, ATTACK_TARGET, &entities, (void**)&attack_target, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (attack_target[i].value == event->entity)
      ecs_rmv(g_ecs, entities[i], ATTACK_TARGET);
  }
}

void update_attack_target_system_init()
{
  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);
}

void update_attack_target_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Vec2          target_position;
  AttackTarget* attack_target;
  Spot*         spot;
  ecs_entity_t  player;
  Vec2          player_position;

  ecs_raw(g_ecs, SPOT, &entities, (void**)&spot, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((attack_target = ecs_get(g_ecs, entities[i], ATTACK_TARGET)))
    {
      target_position = get_entity_position(g_ecs, attack_target->value);
      if (vec2_dist(target_position, spot[i].position) > spot[i].radius)
      {
        ecs_rmv(g_ecs, entities[i], ATTACK_TARGET);
      }
    }
    else if ((player = get_player(g_ecs)) != ECS_NULL_ENT)
    {
      player_position = get_entity_position(g_ecs, player);
      if (vec2_dist(player_position, spot[i].position) <= spot[i].radius)
      {
        ecs_set(g_ecs, entities[i], ATTACK_TARGET, &(AttackTarget){ player });
      }
    }
  }
}
