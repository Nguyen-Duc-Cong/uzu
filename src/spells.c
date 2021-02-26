#include "entity_factory.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"

static void cast_fire_ball(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);
static void cast_ice_arrow(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);

const Spell g_spell_tbl[NUM_SPELLS] = {
  { "ice arrow", cast_ice_arrow, 1, 15, CAST_EFFECT_ICE, { TEX_ICON_ICE_ARROW, { 0, 0, 16, 16 } } },
  { "fire ball", cast_fire_ball, 1, 10, CAST_EFFECT_FIRE, { TEX_ICON_FIRE_BALL, { 0, 0, 16, 16 } } },
};

ecs_entity_t (*const g_cast_effect_fn_tbl[NUM_CAST_EFFECTS])(Ecs* ecs, Vec2 pos) = {
  [CAST_EFFECT_ICE]  = make_fx_cast_ice,
  [CAST_EFFECT_FIRE] = make_fx_cast_fire,
};

#define play_sound(__sound_id) Mix_PlayChannel(-1, get_sfx(__sound_id), 0)

static void cast_fire_ball(Ecs* ecs, SDL_UNUSED ecs_entity_t caster, ecs_entity_t weapon)
{
  WeaponAttributes* attributes;
  Transform*        transform;
  Controller* controller;

  transform = ecs_get(ecs, weapon, TRANSFORM);
  controller = ecs_get(ecs, caster, CONTROLLER);


  Vec2 speed = vec2_mul(controller->attack_direction, 120.f);

  attributes = ecs_get(ecs, weapon, WEAPON_ATTRIBUTES);

  make_fire_ball(ecs, caster, transform->position, speed, attributes->mask);
  play_sound(SFX_FIRE_BALL_LAUCH);
}

static void cast_ice_arrow(Ecs* ecs, SDL_UNUSED ecs_entity_t caster, ecs_entity_t weapon)
{
  WeaponAttributes* attributes;
  Transform*        transform;

  transform = ecs_get(ecs, weapon, TRANSFORM);

  Vec2 speed = { 100.f * transform->hdir, 0.f };

  attributes = ecs_get(ecs, weapon, WEAPON_ATTRIBUTES);

  make_ice_arrow(ecs, caster, transform->position, speed, attributes->mask);

  play_sound(SFX_ICE_SHOOT);
}

#undef play_sound
