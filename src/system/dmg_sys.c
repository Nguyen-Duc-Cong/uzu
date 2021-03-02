#include "components.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"
#include "toolbox/toolbox.h"

extern Ecs* g_ecs;

static const Vec2 _trap_impact_force = { 100.f, 0.f };

#define TRAP_IMPACT_FORCE_Z 60.f

static void on_hit_trap(SDL_UNUSED void* arg, const MSG_EntityHitTrap* event)
{
  const FacingDirection* facing_direction = ecs_get(g_ecs, event->entity, FACING_DIRECTION);
  Vec2                   force = vec2_mul(_trap_impact_force, signf(facing_direction->value.x));
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){ .damage      = 1,
                                   .dealer      = ECS_NULL_ENT,
                                   .receiver    = event->entity,
                                   .type        = DAMAGE_TYPE_THUST,
                                   .impact      = TRUE,
                                   .impact_time = 10,
                                   .force       = force,
                                   .zforce      = TRAP_IMPACT_FORCE_Z });
}

static void on_projectile_hit(SDL_UNUSED void* arg, const MSG_ProjectileHit* event)
{
  ProjectileAttributes* attributes;
  if ((attributes = ecs_get(g_ecs, event->projectile, PROJECTILE_ATTRIBUTES)) != NULL)
  {
    ems_broadcast(MSG_DEAL_DAMAGE,
                  &(MSG_DealDamage){
                      .damage      = attributes->damage,
                      .dealer      = attributes->shooter,
                      .receiver    = event->entity,
                      .type        = attributes->damage_type,
                      .impact      = attributes->impact,
                      .force       = attributes->impact_force,
                      .impact_time = attributes->impact_time,
                      .zforce      = attributes->impact_force_z,
                  });
    if (attributes->destroy_when_hit)
    {
      ecs_add(g_ecs, event->projectile, DESTROYED_TAG);
    }
  }
}

void damage_system_init()
{
  ems_connect(MSG_HIT_TRAP, NULL, on_hit_trap);
  ems_connect(MSG_PROJECTILE_HIT, NULL, on_projectile_hit);
}
