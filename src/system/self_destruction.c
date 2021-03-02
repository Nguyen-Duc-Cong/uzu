#include "system/game_logic.h"

#include "components.h"
#include "entity_factory.h"
#include "include/entity_utils.h"
#include "resources.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

#define EMIT_PARTICLE_DISTANCE 100

void self_destruction_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  SelfDestruction* self_destruction;

  Vec2  position;
  Vec2  target_position;
  float distance;
  Vec2  rposition;
  Vec2  vdir;
  Vec2  force;

  ecs_entity_t player = get_player(g_ecs);

  if (player == ECS_NULL_ENT)
    return;

  ecs_raw(g_ecs, SELF_DESTRUCTION, &entities, (void**)&self_destruction, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    position        = get_entity_position(g_ecs, entities[i]);
    target_position = get_entity_position(g_ecs, player);
    rposition       = vec2_sub(position, target_position);
    distance        = vec2_mag(rposition);
    vdir            = vec2_unit(rposition);

    force = vec2_mul(vdir, -150.f);
    if (distance < self_destruction[i].range)
    {
      ems_broadcast(MSG_DEAL_DAMAGE,
                    &(MSG_DealDamage){ .dealer      = entities[i],
                                       .damage      = 5,
                                       .type        = DAMAGE_TYPE_FIRE,
                                       .receiver    = player,
                                       .impact      = TRUE,
                                       .force       = force,
                                       .impact_time = 10 });
      ecs_add(g_ecs, entities[i], DESTROYED_TAG);

      Mix_PlayChannel(-1, get_sfx(SFX_EXPOLOSION), 0);
    }
  }
}
