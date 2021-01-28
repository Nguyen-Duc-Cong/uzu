#include "charge.h"
#include <components.h>

#define SYS_WEAPON_SKILL_CHARGE_TIME 60

extern Ecs* g_ecs;

void charge_weapon_skl_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponAttributes*   weapon_core;
  Controller*   wearer_controller;
  Visual*       visual;
  Motion*       weaer_motion;
  wpskl_Charge* skl;
  Transform*    transform;
  DamageOutput* damage_output;

  ecs_raw(g_ecs, WEAPON_SKILL_CHARGE, &entities, (void**)&skl, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (skl[i].is_active)
    {
      if (++skl[i].timer == SYS_WEAPON_SKILL_CHARGE_TIME)
      {
        if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
            (weapon_core = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES)) &&
            (weaer_motion = ecs_get(g_ecs, weapon_core->wearer, MOTION)) &&
            (wearer_controller = ecs_get(g_ecs, weapon_core->wearer, CONTROLLER)) &&
            (damage_output = ecs_get(g_ecs, entities[i], DAMAGE_OUTPUT)))
        {
          transform->rot               = 0.0;
          weaer_motion->vel            = VEC2_ZERO;
          wearer_controller->in_action = FALSE;
          skl[i].is_active             = FALSE;
          damage_output->atk           = 0;
        }
      }
      else if ((weapon_core = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES)) &&
               (weaer_motion = ecs_get(g_ecs, weapon_core->wearer, MOTION)) &&
               (visual = ecs_get(g_ecs, entities[i], VISUAL)))
      {
        weaer_motion->acc =
            vec2_mul(VEC2_RIGHT, FLIP_TO_SIGN(visual->flip) * weaer_motion->max_force);
      }
    }
    else if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
             (weapon_core = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES)) &&
             (weaer_motion = ecs_get(g_ecs, weapon_core->wearer, MOTION)) &&
             (wearer_controller = ecs_get(g_ecs, weapon_core->wearer, CONTROLLER)) &&
             (visual = ecs_get(g_ecs, entities[i], VISUAL)) &&
             (damage_output = ecs_get(g_ecs, entities[i], DAMAGE_OUTPUT)) &&
             (weaer_motion = ecs_get(g_ecs, weapon_core->wearer, MOTION)))
    {
      if (wearer_controller->action == skl[i].on_action)
      {
        transform->rot               = 90.0 * FLIP_TO_SIGN(visual->flip);
        damage_output->atk           = weapon_core->atk;
        damage_output->type          = DAMAGE_TYPE_THUST;
        wearer_controller->action    = CHARACTER_ACTION_NONE;
        wearer_controller->in_action = TRUE;
        skl[i].is_active             = TRUE;
        skl[i].timer                 = 0;
        weaer_motion->acc =
            vec2_mul(VEC2_RIGHT, FLIP_TO_SIGN(visual->flip) * weaer_motion->max_force);
      }
    }
  }
}
