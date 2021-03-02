#include "components.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void equipment_system()
{
  ecs_entity_t*     ett;
  Equipment*        equiments;
  Transform*        transform;
  Visual*           visual;
  Transform*        weapon_transform;
  Visual*           weapon_visual;
  ecs_size_t        cnt;
  ecs_entity_t      weapon;
  WeaponAttributes* weapon_attrs;
  Controller*       controller;
  int               sign;

  ecs_raw(g_ecs, EQUIPMENT, &ett, (void**)&equiments, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    weapon = equiments[i].weapon;
    if (weapon == ECS_NULL_ENT)
      continue;

    transform = ecs_get(g_ecs, ett[i], TRANSFORM);
    visual    = ecs_get(g_ecs, ett[i], VISUAL);

    weapon_transform = ecs_get(g_ecs, weapon, TRANSFORM);
    weapon_visual    = ecs_get(g_ecs, weapon, VISUAL);
    weapon_attrs     = ecs_get(g_ecs, weapon, WEAPON_ATTRIBUTES);

    if (transform && visual && weapon_transform && weapon_visual && weapon_attrs)
    {

      sign = transform->horizontal_axis;

      weapon_transform->position.x =
          transform->position.x + sign * (equiments[i].attach_point.x + equiments[i].d.x);
      weapon_transform->position.y =
          transform->position.y + equiments[i].attach_point.y + equiments[i].d.y - transform->z;
      weapon_transform->horizontal_axis = transform->horizontal_axis;
      if (!weapon_attrs->sync_with_attack_direction)
        weapon_visual->flip = visual->flip;

      if (weapon_attrs->sync_with_attack_direction &&
          (controller = ecs_get(g_ecs, ett[i], CONTROLLER)))
      {
        weapon_transform->rotation =
            57.2957795 * SDL_atan2f(controller->attack_direction.y, controller->attack_direction.x);
      }
    }
  }
}
