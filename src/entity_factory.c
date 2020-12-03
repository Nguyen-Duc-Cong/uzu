#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "toolbox/sprite.h"
#include <entity_factory.h>

ecs_entity_t make_anime_sword(Ecs* ecs)
{
  SDL_Texture* texture;
  ecs_entity_t sword;

  /*component*/
  Visual*       visual;
  Transform*    transform;
  GenericSword* generic_sword;
  WeaponAction* cmd_input;
  HitBox*       hitbox;

  texture = get_texture(TEX_ANIME_SWORD);

  sword = ecs_create(ecs);

  transform = ecs_add(ecs, sword, TRANSFORM);

  visual = ecs_add(ecs, sword, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  generic_sword = ecs_add(ecs, sword, GENERIC_SWORD);
  generic_sword->atk = 1;
  generic_sword->range = 15;
  generic_sword->step = 0;
  generic_sword->timer = 0;

  cmd_input = ecs_add(ecs, sword, WEAPON_ACTION);
  cmd_input->action = WEAPON_ACTION_NONE;

  hitbox = ecs_add(ecs, sword, HITBOX);
  hitbox->size = VEC2(12.f, 30.f);
  hitbox->anchor = VEC2(6.f, 30.f);
  hitbox->proxy_id = NULL_NODE;

  return sword;
}

ecs_entity_t make_knight(Ecs* ecs, ecs_entity_t weapon)
{
  ecs_entity_t knight;
  SDL_Texture* texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform*       transform;
  Visual*          visual;
  Equipment*       equipment;
  CharacterAction* cmd_input;
  Animator*        animator;
  HitBox*          hitbox;

  texture = get_texture(TEX_KNIGHT);
  animation_init(&anims[ANIM_STATE_HIT], texture, 0, 0, 1, 1, 16, 28);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 16 * 1, 0, 1, 4, 16, 28);
  animation_init(&anims[ANIM_STATE_RUN], texture, 16 * 5, 0, 1, 4, 16, 28);

  anims[ANIM_STATE_IDLE].frame_duration = 3;
  anims[ANIM_STATE_RUN].frame_duration = 2;

  knight = ecs_create(ecs);

  transform = ecs_add(ecs, knight, TRANSFORM);

  visual = ecs_add(ecs, knight, VISUAL);

  visual->anchor.x = 16 / 2;
  visual->anchor.y = 28 / 2;

  equipment = ecs_add(ecs, knight, EQUIPMENT);
  equipment->rhand = weapon;

  cmd_input = ecs_add(ecs, knight, CHARACTER_ACTION);

  animator = ecs_add(ecs, knight, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_IDLE;
  animator->elapsed = 0;

  hitbox = ecs_add(ecs, knight, HITBOX);
  hitbox->size = VEC2(10.f, 18.f);
  hitbox->anchor = VEC2(5.f, 6.f);
  hitbox->proxy_id = NULL_NODE;
  return knight;
}

ecs_entity_t make_huge_demon(Ecs* ecs, ecs_entity_t weapon)
{
  ecs_entity_t demon;
  SDL_Texture* texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform*       transform;
  Visual*          visual;
  CharacterAction* cmd_input;
  Animator*        animator;
  HitBox*          hitbox;
  Heath*           heath;
  HealBar*         healthBar;
  LifeSpan*        life_span;

  texture = get_texture(TEX_BIG_DEMON);
  animation_init(&anims[ANIM_STATE_HIT], texture, 32 * 6, 0, 1, 1, 32, 36);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 0, 0, 1, 4, 32, 36);
  animation_init(&anims[ANIM_STATE_RUN], texture, 128, 0, 1, 4, 32, 36);

  anims[ANIM_STATE_IDLE].frame_duration = 6;
  anims[ANIM_STATE_RUN].frame_duration = 3;

  demon = ecs_create(ecs);

  transform = ecs_add(ecs, demon, TRANSFORM);
  // transform->rot = 45.0;

  visual = ecs_add(ecs, demon, VISUAL);

  visual->anchor.x = 32 / 2;
  visual->anchor.y = 36 / 2;

  cmd_input = ecs_add(ecs, demon, CHARACTER_ACTION);
  cmd_input->action = WEAPON_ACTION_NONE;

  animator = ecs_add(ecs, demon, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_IDLE;
  animator->elapsed = 0;

  hitbox = ecs_add(ecs, demon, HITBOX);
  hitbox->size = VEC2(32.f, 36.f);
  hitbox->anchor = VEC2(16.f, 18.f);
  hitbox->proxy_id = NULL_NODE;

  ecs_add(ecs, demon, ENEMY_TAG);

  heath = ecs_add(ecs, demon, HEATH);
  heath->max_hit_points = 100;
  heath->hit_points = 60;

  healthBar = ecs_add(ecs, demon, HEAL_BAR);
  healthBar->len = 40;
  healthBar->anchor = (SDL_Point){ 20, 25 };

  return demon;
}

ecs_entity_t make_axe(Ecs* ecs)
{
  SDL_Texture* texture;
  ecs_entity_t axe;

  /*component*/
  Visual*       visual;
  Transform*    transform;
  GenericAxe*   generic_axe;
  WeaponAction* cmd_input;

  texture = get_texture(TEX_AXE);

  axe = ecs_create(ecs);

  transform = ecs_add(ecs, axe, TRANSFORM);

  visual = ecs_add(ecs, axe, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  generic_axe = ecs_add(ecs, axe, GENERIC_AXE);
  generic_axe->atk = 1;
  generic_axe->step = 0;
  generic_axe->timer = 0;

  cmd_input = ecs_add(ecs, axe, WEAPON_ACTION);
  cmd_input->action = WEAPON_ACTION_NONE;

  return axe;
}

ecs_entity_t make_blood_stain_effect(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t stain_effect;

  Visual*    visual;
  Animator*  animator;
  Transform* transform;
  LifeSpan*  life_span;

  Animation    animation;
  SDL_Texture* texture;
  texture = get_texture(TEX_BLOOD);
  animation_init(&animation, texture, 0, 0, 1, 4, 16, 16);

  stain_effect = ecs_create(ecs);

  visual = ecs_add(ecs, stain_effect, VISUAL);
  visual->anchor = (SDL_Point){ 8, 8 };

  animator = ecs_add(ecs, stain_effect, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform = ecs_add(ecs, stain_effect, TRANSFORM);
  transform->pos = pos;

  life_span = ecs_add(ecs, stain_effect, LIFE_SPAN);
  life_span->remaining = 15; // frames

  return stain_effect;
}

ecs_entity_t make_bow(Ecs* ecs)
{
  ecs_entity_t bow;

  Visual*       visual;
  Transform*    transform;
  WeaponAction* cmd;
  GenericBow*   g_bow;

  SDL_Texture* texture;
  texture = get_texture(TEX_BOW);

  bow = ecs_create(ecs);

  visual = ecs_add(ecs, bow, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h / 2;

  transform = ecs_add(ecs, bow, TRANSFORM);

  cmd = ecs_add(ecs, bow, WEAPON_ACTION);

  g_bow = ecs_add(ecs, bow, GENERIC_BOW);

  return bow;
}

ecs_entity_t make_arrow(Ecs* ecs, Vec2 pos, Vec2 vel)
{
  SDL_Texture* texture;
  ecs_entity_t arrow;

  Transform*  transform;
  Visual*     visual;
  Motion*     motion;
  Projectile* projectile;
  HitBox*     hitbox;

  texture = get_texture(TEX_ARROW);

  arrow = ecs_create(ecs);

  transform = ecs_add(ecs, arrow, TRANSFORM);
  transform->pos = pos;

  motion = ecs_add(ecs, arrow, MOTION);
  motion->vel = vel;
  motion->acc = VEC2(0.f, 0.f);

  visual = ecs_add(ecs, arrow, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual_set_anchor_to_center(visual);

  projectile = ecs_add(ecs, arrow, PROJECTILE);
  projectile->atk = 3;

  hitbox = ecs_add(ecs, arrow, HITBOX);
  hitbox->size.x = visual->sprite.rect.w;
  hitbox->size.y = 3;
  hitbox->anchor.x = hitbox->size.x / 2.f;
  hitbox->anchor.y = hitbox->size.y / 2.f;
  hitbox->proxy_id = NULL_NODE;

  ecs_add(ecs, arrow, PLAYER_WEAPON_TAG);

  return arrow;
}

ecs_entity_t make_golden_sword(Ecs* ecs)
{
  ecs_entity_t entity;

  SDL_Texture* texture;

  entity = ecs_create(ecs);
  texture = get_texture(TEX_GOLDEN_SWORD);

  Transform*    transform;
  Visual*       visual;
  WeaponAction* weapon_action;
  GoldenSword*  golden_sword;
  HitBox*       hitbox;

  transform = ecs_add(ecs, entity, TRANSFORM);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  weapon_action = ecs_add(ecs, entity, WEAPON_ACTION);
  weapon_action->action = WEAPON_ACTION_NONE;

  hitbox = ecs_add(ecs, entity, HITBOX);
  hitbox->size = VEC2(visual->sprite.rect.w, visual->sprite.rect.h);
  hitbox->anchor = VEC2(visual->anchor.x, visual->anchor.y);
  hitbox->proxy_id = NULL_NODE;

  golden_sword = ecs_add(ecs, entity, GOLDEN_SWORD);

  return entity;
}

ecs_entity_t make_golden_cross_hit_effect(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t entity;
  SDL_Texture* texture;
  Animation    animation;

  Transform* transform;
  Visual*    visual;
  Animator*  animator;
  LifeSpan* life_span;

  texture = get_texture(TEX_GOLDEN_CROSS_HIT);
  animation_init(&animation, texture, 0, 0, 1, 10, 48, 48);

  entity = ecs_create(ecs);

  transform = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  visual = ecs_add(ecs, entity, VISUAL);
  visual->anchor = (SDL_Point){ 24, 24 };

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  life_span = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = animation.sheet.count * animation.frame_duration;

  return entity;
}
