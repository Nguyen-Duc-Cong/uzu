#include "entity_factory.h"
#include "animations.h"
#include "behaviour_tree.h"
#include "btcondition.h"
#include "bttask.h"
#include "components.h"
#include "constances.h"
#include "entity_utils.h"
#include "ecs/ecs.h"
#include "resources.h"

ecs_entity_t make_anime_sword(Ecs* registry, u16 mask)
{

  ecs_entity_t entity;

  entity = ecs_create(registry);

  Transform*         transform;
  Visual*            visual;
  WeaponAttributes*  attrs;
  WeaponSwingAttack* sklswing;

  transform = ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_ANIME_SWORD);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h + 4;

  attrs          = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attrs->atk     = 2;
  attrs->type_id = WEAPON_ANIME_SWORD;

  sklswing            = ecs_add(registry, entity, WEAPON_SWING_ATTACK);
  sklswing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  sklswing->range     = 40;

  ecs_add(registry, entity, HOLDER);

  return entity;
}

#define CHARACTER_SPRITE_WIDTH 16
#define CHARACTER_SPRITE_HEIGHT 28

typedef struct NewCharacterParams
{
  Vec2             position;
  const Animation* animations;
  u16              hit_points;
  u16              mana_points;
} NewCharacterParams;

ecs_entity_t make_character_base(Ecs* registry, const NewCharacterParams* params)
{
  ecs_entity_t entity;

  /*components */
  Transform*  transform;
  Visual*     visual;
  Hand*       hand;
  Controller* controller;
  Animator*   animator;
  HitBox*     hitbox;
  Motion*     motion;
  Health*     heath;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 16 / 2;
  visual->anchor.y = 28;

  hand                   = ecs_add(registry, entity, HAND);
  hand->weapon           = ECS_NULL_ENT;
  hand->attach_point.x   = 8;
  hand->attach_point.y   = 0;
  hand->original_point.x = 0;
  hand->original_point.y = -7;

  controller = ecs_add(registry, entity, CONTROLLER);

  animator               = ecs_add(registry, entity, ANIMATOR);
  animator->current_anim = ANIM_STATE_IDLE;
  animator->elapsed      = 0;
  animator->anims        = params->animations;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = VEC2(6.f, 10.f);
  hitbox->anchor   = VEC2(3.f, 10.f);
  hitbox->proxy_id = RTREE_NULL_NODE;

  motion                = ecs_add(registry, entity, MOTION);
  heath                 = ecs_add(registry, entity, HEALTH);
  heath->hit_points     = params->hit_points;
  heath->max_hit_points = params->hit_points;

  ecs_add(registry, entity, ENABLE_TILE_COLLISION_TAG);
  ecs_add(registry, entity, CHARACTER_ANIMATOR_TAG);

  ecs_set(registry,
          entity,
          MANA_POOL,
          &(ManaPool){
              params->mana_points,
              params->mana_points,
          });

  ecs_set(registry, entity, MOVE_SPEED, &(MoveSpeed){ 120 });
  ecs_add(registry, entity, FACING_DIRECTION);
  return entity;
}
extern const u16 g_initial_hp_tbl[];
extern const u16 g_initial_mp_tbl[];

ecs_entity_t make_knight(Ecs* registry, Vec2 position)
{
  NewCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_knight_m;
  params.hit_points  = g_initial_hp_tbl[JOB_KNIGHT];
  params.mana_points = g_initial_mp_tbl[JOB_KNIGHT];
  return make_character_base(registry, &params);
}

ecs_entity_t make_elf(Ecs* registry, Vec2 position)
{
  NewCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_elf_m;
  params.hit_points  = g_initial_hp_tbl[JOB_ELF];
  params.mana_points = g_initial_mp_tbl[JOB_ELF];
  return make_character_base(registry, &params);
}

ecs_entity_t make_wizzard(Ecs* registry, Vec2 position)
{
  NewCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_wizzard_m;
  params.hit_points  = g_initial_hp_tbl[JOB_WIZZARD];
  params.mana_points = g_initial_mp_tbl[JOB_WIZZARD];
  return make_character_base(registry, &params);
}

ecs_entity_t make_lizzard(Ecs* registry, Vec2 position)
{
  NewCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_lizzard_m;
  params.hit_points  = g_initial_hp_tbl[JOB_DRAGON];
  params.mana_points = g_initial_mp_tbl[JOB_DRAGON];
  return make_character_base(registry, &params);
}

typedef struct NewMonsterParams
{
  Vec2             position;
  const Animation* anims;
  u16              hit_points;
  Vec2             size;
} NewMonsterParams;

static const Vec2 s_big_size  = { 20.f, 20.f };
static const Vec2 _small_size = { 10.f, 12.f };

ecs_entity_t make_monster_base(Ecs* registry, const NewMonsterParams* params)
{
  ecs_entity_t entity;

  /*components */
  Transform* transform;
  Visual*    visual;
  Animator*  animator;
  HitBox*    hitbox;
  Health*    heath;
  HealthBar* health_bar;
  Motion*    motion;

  const int sprite_width  = params->anims[0].sprite_width;
  const int sprite_height = params->anims[0].sprite_height;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  visual = ecs_add(registry, entity, VISUAL);

  visual->anchor.x = params->anims[0].sprite_width / 2;
  visual->anchor.y = params->anims[0].sprite_height;

  animator               = ecs_add(registry, entity, ANIMATOR);
  animator->current_anim = ANIM_STATE_RUN;
  animator->elapsed      = 0;
  animator->anims        = params->anims;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size      = params->size;
  hitbox->anchor    = VEC2(params->size.x / 2.f, params->size.y);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->mask_bits = BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE);
  hitbox->category  = CATEGORY_ENEMY;

  ecs_add(registry, entity, ENEMY_TAG);

  heath                 = ecs_add(registry, entity, HEALTH);
  heath->max_hit_points = params->hit_points;
  heath->hit_points     = params->hit_points;

  health_bar         = ecs_add(registry, entity, HEAL_BAR);
  health_bar->len    = params->anims[0].sprite_width;
  health_bar->anchor = (SDL_Point){ sprite_width / 2, sprite_height };
  health_bar->color  = (SDL_Color){ 0x42, 0xde, 0x23, 0xd0 };

  motion = ecs_add(registry, entity, MOTION);

  ecs_add(registry, entity, ENABLE_TILE_COLLISION_TAG);
  ecs_add(registry, entity, CHARACTER_ANIMATOR_TAG);
  ecs_set(registry, entity, MOVE_SPEED, &(MoveSpeed){ 50 });
  ecs_add(registry, entity, FACING_DIRECTION);
  ecs_set(registry, entity, ATTACK_MASK, &(AttackMask){ BIT(CATEGORY_PLAYER) });
  return entity;
}

ecs_entity_t make_huge_demon(Ecs* registry, Vec2 position)
{
  return make_monster_base(registry,
                           &(NewMonsterParams){ position, g_anims_huge_demon, 30, s_big_size });
}

ecs_entity_t make_imp(Ecs* registry, Vec2 position)
{
  ecs_entity_t entity = make_monster_base(registry,
                                          &(NewMonsterParams){ .position   = position,
                                                               .anims      = g_anims_imp,
                                                               .hit_points = 5,
                                                               .size       = _small_size });
  ecs_set(registry,
          entity,
          SELF_DESTRUCTION,
          &(SelfDestruction){ .range = 16, .target = ECS_NULL_ENT });

  ecs_set(registry, entity, AGGRO_AREA, &(AggroArea){ .position = position, .radius = 150.f });
  return entity;
}

ecs_entity_t make_wogol(Ecs* registry, Vec2 position)
{
  return make_monster_base(registry,
                           &(NewMonsterParams){ position, g_anims_wogol, 5, _small_size });
}

struct AnimatedPickupableEntityParams
{
  u16              id;
  u16              quality;
  const Animation* animation;
  Vec2             position;
};
ecs_entity_t make_animated_pickupable_entity(Ecs*                                         registry,
                                             const struct AnimatedPickupableEntityParams* params)
{
  ecs_entity_t entity;

  Visual*               visual;
  Transform*            transform;
  HitBox*               hitbox;
  PickupableAttributes* attrs;
  Animator*             animator;

  entity = ecs_create(registry);

  visual         = ecs_add(registry, entity, VISUAL);
  visual->anchor = (POINT){ 4, 4 };

  animator        = ecs_add(registry, entity, ANIMATOR);
  animator->anims = params->animation;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size      = (Vec2){ 8.f, 8.f };
  hitbox->anchor    = (Vec2){ 4.f, 4.f };
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->category  = CATEGORY_ITEM;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);

  attrs          = ecs_add(registry, entity, PICKUPABLE_ATTRIBUTES);
  attrs->id      = params->id;
  attrs->sfx     = SFX_COIN;
  attrs->quality = params->quality;

  ecs_set(registry, entity, MOTION, &(Motion){ .friction = .2f, .bounching = .8f });
  return entity;
}

static BTNode* create_chort_behavior_tree(void)
{
  BTRoot* root;

  BTSelector*                  selector;
  BTCondition_HasAttackTarget* has_attack_target;
  BTSequence*                  sequence1;
  BTTask_Wait*                 wait;
  BTTask_FollowAttackTarget*   follow_attack_target;
  BTTask_Attack*               attack;

  root                 = bt_root_new();
  selector             = bt_selector_new();
  has_attack_target    = bt_condition_has_attack_target_new(FALSE);
  sequence1            = bt_sequence_new();
  wait                 = bt_task_wait_new(30);
  attack               = bt_task_attack_new();
  follow_attack_target = bt_task_follow_attack_target_new(16);

  bt_root_set_child(root, (BTNode*)selector);

  bt_selector_add(selector, (BTNode*)has_attack_target);

  bt_decorator_set_child((BTDecorator*)has_attack_target, (BTNode*)sequence1);
  bt_sequence_add(sequence1, (BTNode*)follow_attack_target);
  bt_sequence_add(sequence1, (BTNode*)attack);
  bt_sequence_add(sequence1, (BTNode*)wait);

  return (BTNode*)root;
}

ecs_entity_t make_chort(Ecs* registry, Vec2 position)
{
  ecs_entity_t entity =
      make_monster_base(registry, &(NewMonsterParams){ position, g_anims_chort, 5, _small_size });

  /*components */
  Drop*       drop;
  Hand*       hand;
  Controller* controller;
  AggroArea*  spot;

  spot           = ecs_add(registry, entity, AGGRO_AREA);
  spot->radius   = TILE_SIZE * 7;
  spot->position = position;

  drop          = ecs_add(registry, entity, DROP);
  drop->type[0] = PICKUPABLE_RED_FLASK;
  drop->type[1] = PICKUPABLE_BLUE_FLASK;
  drop->type[2] = PICKUPABLE_COIN;

  drop->rate[0] = 70;
  drop->rate[1] = 60;
  drop->rate[2] = 100;

  drop->quality[0] = 1;
  drop->quality[1] = 1;
  drop->quality[2] = 1;

  drop->cnt = 3;

  controller = ecs_add(registry, entity, CONTROLLER);

  hand                 = ecs_add(registry, entity, HAND);
  hand->weapon         = ECS_NULL_ENT;
  hand->attach_point.x = 16 / 2;
  hand->attach_point.y = -6;

  ecs_entity_t weapon = make_spear(registry, BIT(CATEGORY_PLAYER));

  equip(registry, entity, weapon);

  ecs_set(registry, entity, BRAIN, &(Brain){ create_chort_behavior_tree() });

  return entity;
}

ecs_entity_t make_axe(Ecs* registry)
{
  ecs_entity_t axe;

  /*component*/
  Visual*    visual;
  Transform* transform;

  axe = ecs_create(registry);

  transform = ecs_add(registry, axe, TRANSFORM);

  visual = ecs_add(registry, axe, VISUAL);
  sprite_init(&visual->sprite, TEX_AXE);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  return axe;
}

ecs_entity_t make_cleaver(Ecs* registry, u16 mask)
{
  ecs_entity_t entity;

  TEXTURE* texture;

  entity  = ecs_create(registry);
  texture = get_texture(TEX_CLEAVER);

  Transform*         transform;
  Visual*            visual;
  WeaponAttributes*  attrs;
  WeaponSwingAttack* sklswing;

  transform = ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_CLEAVER);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  attrs          = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attrs->atk     = 2;
  attrs->type_id = WEAPON_CLEAVER;

  sklswing            = ecs_add(registry, entity, WEAPON_SWING_ATTACK);
  sklswing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  sklswing->range     = 20;

  ecs_add(registry, entity, HOLDER);

  return entity;
}

ecs_entity_t make_golden_sword(Ecs* registry, u16 mask)
{
  ecs_entity_t entity;

  entity = ecs_create(registry);

  Transform*                 transform;
  Visual*                    visual;
  WeaponAttributes*          attrs;
  WeaponSwingAttack*         sklswing;
  WeaponThunderStormRelease* wpskl_thunder_storm;

  transform = ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_GOLDEN_SWORD);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  attrs          = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attrs->atk     = 2;
  attrs->type_id = WEAPON_LAVIS_SWORD;

  sklswing            = ecs_add(registry, entity, WEAPON_SWING_ATTACK);
  sklswing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  sklswing->range     = 100;

  wpskl_thunder_storm            = ecs_add(registry, entity, WEAPON_THUNDER_STORM_RELEASE);
  wpskl_thunder_storm->on_action = CHARACTER_ACTION_SPECIAL_ATK;
  wpskl_thunder_storm->interval  = 10;
  wpskl_thunder_storm->total     = 5;

  ecs_add(registry, entity, HOLDER);

  return entity;
}

ecs_entity_t make_big_red_flask(Ecs* registry, Vec2 position, u8 quality)
{
  return make_static_pickupable_entity(registry,
                                       TEX_FLASK_RED_BIG,
                                       PICKUPABLE_BIG_RED_FLASK,
                                       position,
                                       quality);
}

ecs_entity_t make_red_flask(Ecs* registry, Vec2 position, u8 quality)
{
  return make_static_pickupable_entity(registry,
                                       TEX_FLASK_RED,
                                       PICKUPABLE_RED_FLASK,
                                       position,
                                       quality);
}

ecs_entity_t make_blue_flask(Ecs* registry, Vec2 position, u8 quality)
{
  return make_static_pickupable_entity(registry,
                                       TEX_BLUE_FLASK,
                                       PICKUPABLE_BLUE_FLASK,
                                       position,
                                       quality);
}

ecs_entity_t
make_static_pickupable_entity(Ecs* registry, u16 texture_id, u16 id, Vec2 position, u8 quality)
{
  ecs_entity_t entity;

  Visual*               visual;
  Transform*            transform;
  HitBox*               hitbox;
  PickupableAttributes* attrs;

  entity = ecs_create(registry);

  visual                    = ecs_add(registry, entity, VISUAL);
  visual->sprite.texture_id = texture_id;
  sprite_init(&visual->sprite, texture_id);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size      = VEC2(visual->sprite.rect.w, visual->sprite.rect.h);
  hitbox->anchor    = VEC2(hitbox->size.x / 2.f, hitbox->size.y / 2.f);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->category  = CATEGORY_ITEM;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);

  attrs          = ecs_add(registry, entity, PICKUPABLE_ATTRIBUTES);
  attrs->id      = id;
  attrs->sfx     = SFX_INTERACTION;
  attrs->quality = quality;

  ecs_set(registry, entity, MOTION, &(Motion){ .friction = .2f, .bounching = .8f });
  return entity;
}

ecs_entity_t make_player(Ecs* registry, ecs_entity_t character, ecs_entity_t weapon)
{
  ecs_add(registry, character, PLAYER_TAG);
  ecs_add(registry, character, CAMERA_TARGET_TAG);

  AttunementSlot* attunement_slot;

  HitBox* hitbox = ecs_get(registry, character, HITBOX);

  equip(registry, character, weapon);

  hitbox->category  = CATEGORY_PLAYER;
  hitbox->mask_bits = BIT(CATEGORY_ITEM) | BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE) |
                      BIT(CATEGORY_LADDER) | BIT(CATEGORY_INTERACABLE);

  attunement_slot           = ecs_add(registry, character, ATTUNEMENT_SLOT);
  attunement_slot->spell_id = SPELL_ID_NULL;

  ecs_set(registry,
          character,
          ATTACK_MASK,
          &(AttackMask){ BIT(CATEGORY_ENEMY) | BIT(CATEGORY_INTERACABLE) });

  return character;
}

ecs_entity_t
make_thunder(SDL_UNUSED Ecs* registry, SDL_UNUSED Vec2 position, SDL_UNUSED u16 mask_bits)
{
  return ECS_NULL_ENT;
}

ecs_entity_t make_ladder(Ecs* registry, const NewLadderParams* params)
{
  ecs_entity_t entity;

  Transform* transform;
  HitBox*    hitbox;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size      = params->size;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);
  hitbox->category  = CATEGORY_LADDER;
  hitbox->anchor.x  = 0;
  hitbox->anchor.y  = 0;

  ladder_attrs_init(ecs_add(registry, entity, LADDER_ATTRIBUTES),
                    params->level,
                    params->dest,
                    params->direction);

  name_init(ecs_add(registry, entity, NAME), params->name);

  return entity;
}

ecs_entity_t make_text_particle(Ecs*        registry,
                                const char* text,
                                Vec2        position,
                                Vec2        vel,
                                FONT*       font,
                                COLOR       color)
{
  ecs_entity_t entity;

  Transform* transform;
  Motion*    motion;
  LifeSpan*  life_span;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  motion      = ecs_add(registry, entity, MOTION);
  motion->vel = vel;

  text_init(ecs_add(registry, entity, TEXT), text, font, color);

  life_span            = ecs_add(registry, entity, LIFE_SPAN);
  life_span->remaining = 30;

  return entity;
}

ecs_entity_t make_fx_damage_indicator(Ecs* registry, Vec2 position, COLOR color, int amount)
{
  char strnum[5];
  sprintf(strnum, "%d", amount);
  return make_text_particle(registry,
                            strnum,
                            position,
                            VEC2(0.f, -30.f),
                            get_font(FONT_DAMAGE_INDICATOR),
                            color);
}

ecs_entity_t make_fx_item_picked_up(Ecs* registry, Vec2 position, const char* item_name)
{
  return make_text_particle(registry,
                            item_name,
                            position,
                            VEC2(0.f, -40.f),
                            get_font(FONT_ITEM_PICKED_UP),
                            (COLOR){ 122, 196, 10, 255 });
}

ecs_entity_t make_chest(Ecs* registry, const NewChestParams* params)
{
  ecs_entity_t entity;

  Chest*        chest;
  Visual*       visual;
  Interactable* interactable;
  HitBox*       hitbox;
  Transform*    transform;

  entity                    = ecs_create(registry);
  visual                    = ecs_add(registry, entity, VISUAL);
  visual->sprite.texture_id = TEX_CHEST;
  visual->sprite.rect = params->state == CHEST_STATE_CLOSE ? RECT_CHEST_CLOSE : RECT_CHEST_CLOSE;
  visual->anchor.x    = 8.f;
  visual->anchor.y    = 16.f;

  chest = ecs_add(registry, entity, CHEST);
  SDL_memcpy(chest->items, params->items, params->num_slots * sizeof(Item));
  chest->num_slots = params->num_slots;
  chest->id        = params->id;
  chest->state     = params->state;

  interactable               = ecs_add(registry, entity, INTERACTABLE);
  interactable->num_commands = 1;
  interactable->commands[0]  = TEXT_COMMAND_OPEN;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size      = (Vec2){ 16, 16 };
  hitbox->anchor    = (Vec2){ 8.f, 16.f };
  hitbox->category  = CATEGORY_INTERACABLE;
  hitbox->mask_bits = 0;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  return entity;
}

ecs_entity_t make_spear(Ecs* registry, u16 mask)
{
  ecs_entity_t entity;

  Visual*            visual;
  Transform*         transform;
  WeaponThustAttack* thust;
  WeaponAttributes*  attrs;

  entity = ecs_create(registry);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_SPEAR);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = 3;

  transform = ecs_add(registry, entity, TRANSFORM);

  thust            = ecs_add(registry, entity, WEAPON_THUST_ATTACK);
  thust->on_action = CHARACTER_ACTION_REGULAR_ATK;

  attrs      = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attrs->atk = 1;

  return entity;
}

ecs_entity_t make_door(Ecs* registry, Vec2 position)
{
  ecs_entity_t entity;

  HitBox*         hitbox;
  Visual*         visual;
  Transform*      transform;
  DoorAttributes* door_info;
  Interactable*   interactable;

  entity = ecs_create(registry);

  visual                    = ecs_add(registry, entity, VISUAL);
  visual->sprite.texture_id = TEX_DOOR;
  visual->sprite.rect       = RECT_DOOR_CLOSE;
  visual->anchor.x          = 16;
  visual->anchor.y          = 34;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size      = (Vec2){ 32, 5 };
  hitbox->anchor    = (Vec2){ 16, 5 };
  hitbox->category  = CATEGORY_INTERACABLE;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER) | BIT(CATEGORY_ENEMY);

  interactable               = ecs_add(registry, entity, INTERACTABLE);
  interactable->num_commands = 1;
  interactable->commands[0]  = TEXT_COMMAND_OPEN;

  door_info               = ecs_add(registry, entity, DOOR_ATTRIBUTES);
  door_info->required_key = ITEM_TYPE_KEY_1_1;
  door_info->state        = DOOR_STATE_CLOSE;

  return entity;
}

static const Animation* _blood_effect_tbl[] = {
  &g_anim_blood_loss_1,
  &g_anim_blood_loss_2,
};

#define NUM_BLOOD_LOSS_EFFECTS 2

ecs_entity_t make_fx_blood_loss(Ecs* registry, Vec2 position)
{
  ecs_entity_t entity;
  Visual*      visual;
  Transform*   transform;
  LifeSpan*    life_span;
  Animator*    animator;

  const int sw = 100;
  const int sh = 100;
  int       i;

  i = rand() % NUM_BLOOD_LOSS_EFFECTS;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  animator = ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = _blood_effect_tbl[i] });

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  life_span = ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 28 });

  return entity;
}

ecs_entity_t make_staff(Ecs* registry, u16 mask)
{
  ecs_entity_t entity = ecs_create(registry);

  Transform*        transform;
  Visual*           visual;
  WeaponAttributes* attrs;
  Castable*         castable;

  transform = ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, (TEX_RED_STAFF));
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h / 2 + 4;

  attrs              = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attrs->atk         = 1;
  attrs->type_id     = WEAPON_STAFF;
  attrs->rotate_hand = FALSE;

  castable = ecs_add(registry, entity, WEAPON_CAST);

  ecs_add(registry, entity, HOLDER);

  return entity;
}

ecs_entity_t make_fire_ball(Ecs* registry, ecs_entity_t shooter, Vec2 pos, Vec2 direction, u16 mask)
{
  ecs_entity_t          entity = ecs_create(registry);
  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  HitBox*               hitbox;
  ProjectileAttributes* attrs;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 15;
  visual->anchor.y = 7;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = pos;
  transform->rotation = SDL_atan2(direction.y, direction.x) * 57.2957795;

  motion           = ecs_add(registry, entity, MOTION);
  motion->vel      = direction;
  motion->friction = 0.f;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size.x    = 26;
  hitbox->size.y    = 10;
  hitbox->anchor.x  = 18;
  hitbox->anchor.y  = 5;
  hitbox->category  = CATEGORY_PROJECTILE;
  hitbox->mask_bits = mask;

  attrs                   = ecs_add(registry, entity, PROJECTILE_ATTRIBUTES);
  attrs->damage           = 3;
  attrs->destroy_when_hit = TRUE;
  attrs->effect           = 0;
  attrs->damage_type      = DAMAGE_TYPE_FIRE;
  attrs->impact           = TRUE;
  attrs->impact_force     = vec2_mul(vec2_unit(direction), 100.f);
  attrs->impact_time      = 16;
  attrs->sfx              = SFX_ID_NULL;
  attrs->shooter          = shooter;

  ecs_add(registry, entity, REMOVE_IF_OFFSCREEN);

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_fire_ball });

  return entity;
}

ecs_entity_t
make_ice_arrow(Ecs* registry, ecs_entity_t shooter, Vec2 position, Vec2 speed, u16 mask)
{
  ecs_entity_t entity = ecs_create(registry);

  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  HitBox*               hitbox;
  ProjectileAttributes* attrs;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 15;
  visual->anchor.y = 7;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;
  transform->rotation = SDL_atan2(speed.y, speed.x) * 57.2957795;

  motion           = ecs_add(registry, entity, MOTION);
  motion->vel      = speed;
  motion->friction = 0.f;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_ice_arrow });

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size.x    = 32;
  hitbox->size.y    = 15;
  hitbox->anchor.x  = 16;
  hitbox->anchor.y  = 7;
  hitbox->category  = CATEGORY_PROJECTILE;
  hitbox->mask_bits = mask;

  attrs                   = ecs_add(registry, entity, PROJECTILE_ATTRIBUTES);
  attrs->damage           = 3;
  attrs->destroy_when_hit = TRUE;
  attrs->effect           = 0;
  attrs->damage_type      = DAMAGE_TYPE_ICE;
  attrs->impact           = TRUE;
  attrs->impact_force     = vec2_mul(vec2_unit(speed), 100.f);
  attrs->impact_time      = 16;
  attrs->sfx              = SFX_ID_NULL;
  attrs->shooter          = shooter;

  ecs_add(registry, entity, REMOVE_IF_OFFSCREEN);

  return entity;
}

ecs_entity_t make_arrow(Ecs* registry, ecs_entity_t shooter, Vec2 position, Vec2 speed, u16 mask)
{
  ecs_entity_t entity = ecs_create(registry);

  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  HitBox*               hitbox;
  ProjectileAttributes* attrs;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 17;
  visual->anchor.y = 2;
  sprite_init(&visual->sprite, TEX_ARROW);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;
  transform->rotation = SDL_atan2(speed.y, speed.x) * 57.2957795;

  motion           = ecs_add(registry, entity, MOTION);
  motion->vel      = speed;
  motion->friction = 0.f;

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size.x    = 24;
  hitbox->size.y    = 5;
  hitbox->anchor.x  = 17;
  hitbox->anchor.y  = 2;
  hitbox->category  = CATEGORY_PROJECTILE;
  hitbox->mask_bits = mask;

  attrs                   = ecs_add(registry, entity, PROJECTILE_ATTRIBUTES);
  attrs->damage           = 2;
  attrs->destroy_when_hit = TRUE;
  attrs->effect           = 0;
  attrs->damage_type      = DAMAGE_TYPE_THUST;
  attrs->impact           = TRUE;
  attrs->impact_force     = vec2_mul(vec2_unit(speed), 100.f);
  attrs->impact_time      = 16;
  attrs->sfx              = SFX_ID_NULL;
  attrs->shooter          = shooter;

  ecs_add(registry, entity, REMOVE_IF_OFFSCREEN);

  return entity;
}

ecs_entity_t make_fx_cast_ice(Ecs* registry, Vec2 pos)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 48;
  visual->anchor.y = 48;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = pos;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_ice_cast });
  ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 29 });
  return entity;
}

ecs_entity_t make_fx_cast_fire(Ecs* registry, Vec2 position)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 48;
  visual->anchor.y = 48;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_fire_cast });
  ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 29 });
  return entity;
}

ecs_entity_t make_fx_slash(Ecs* registry, Vec2 position, SDL_RendererFlip flip)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;

  const int sw = 48;
  const int sh = 48;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;
  visual->flip     = flip;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_slash });
  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(registry, entity, LIFE_SPAN);
  life_span->remaining = 10;

  return entity;
}

ecs_entity_t make_fx_fire_hit(Ecs* registry, Vec2 position)
{

  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;

  const int sw = 64;
  const int sh = 64;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_fire_hit });
  ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 44 });

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  return entity;
}
ecs_entity_t make_fx_ice_hit(Ecs* registry, Vec2 position)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;

  const int sw = 96;
  const int sh = 96;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_ice_hit });

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(registry, entity, LIFE_SPAN);
  life_span->remaining = 49;

  return entity;
}

ecs_entity_t make_npc_nova(Ecs* registry, Vec2 position, u16 conversation_id)
{
  ecs_entity_t  entity = ecs_create(registry);
  HitBox*       hitbox;
  Interactable* interactable;
  Animator*     animator;
  Visual*       visual;

  ecs_set(registry, entity, TRANSFORM, &(Transform){ .position = position });
  ecs_set(registry, entity, DIALOGUE, &(Dialogue){ conversation_id });
  ecs_set(registry, entity, NAME, &(Name){ .value = "nova" });
  ecs_set(registry,
          entity,
          ATTACK_MASK,
          &(AttackMask){ BIT(CATEGORY_PLAYER) | BIT(CATEGORY_ENEMY) });
  ecs_set(registry, entity, HEALTH, &(Health){ 15, 15 });
  ecs_set(registry,
          entity,
          HEAL_BAR,
          &(HealthBar){ .color = { 0x03, 0xb6, 0xfc, 0xff }, .len = 20, .anchor = { 6, 23 } });
  ecs_set(registry, entity, MOTION, &(Motion){ 0 });
  ecs_add(registry, entity, CHARACTER_ANIMATOR_TAG);

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->category  = CATEGORY_INTERACABLE;
  hitbox->mask_bits = BIT(CATEGORY_PROJECTILE);
  hitbox->size.x    = 13;
  hitbox->size.y    = 19;
  hitbox->anchor.x  = 6;
  hitbox->anchor.y  = 19;

  interactable               = ecs_add(registry, entity, INTERACTABLE);
  interactable->num_commands = 1;
  interactable->commands[0]  = TEXT_COMMAND_TALK;

  animator        = ecs_add(registry, entity, ANIMATOR);
  animator->anims = g_anims_elite_knight;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 16;
  visual->anchor.y = 32;

  ecs_add(registry, entity, CONTROLLER);

  ecs_set(registry, entity, HAND, &(Hand){ .weapon = ECS_NULL_ENT, .attach_point = { 8, -9 } });

  ecs_add(registry, entity, ENABLE_TILE_COLLISION_TAG);
  ecs_set(registry, entity, MOVE_SPEED, &(MoveSpeed){ 120 });
  return entity;
}

ecs_entity_t make_trigger(Ecs* registry, Vec2 pos, Vec2 size, u16 mask)
{
  ecs_entity_t entity;

  HitBox* hitbox;

  entity = ecs_create(registry);

  hitbox            = ecs_add(registry, entity, HITBOX);
  hitbox->size      = size;
  hitbox->mask_bits = mask;
  hitbox->category  = CATEGORY_TRIGGER;

  ecs_set(registry, entity, TRANSFORM, &(Transform){ .position = pos });

  return entity;
}

ecs_entity_t make_coin(Ecs* registry, Vec2 position, u8 quality)
{
  make_animated_pickupable_entity(registry,
                                  &(struct AnimatedPickupableEntityParams){
                                      .id        = PICKUPABLE_COIN,
                                      .quality   = quality,
                                      .position  = position,
                                      .animation = &g_anim_coin,
                                  });
}

ecs_entity_t make_key_1_1(Ecs* registry, Vec2 pos, u8 quality)
{
  return make_static_pickupable_entity(registry, TEX_KEY, PICKUPABLE_KEY_1_1, pos, quality);
}

ecs_entity_t make_bow(Ecs* registry, u16 mask)
{
  ecs_entity_t entity;

  entity = ecs_create(registry);

  Transform*        transform;
  Visual*           visual;
  WeaponAttributes* attrs;

  transform = ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_BOW);
  visual->anchor.x = 0;
  visual->anchor.y = visual->sprite.rect.h / 2;

  attrs              = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attrs->atk         = 2;
  attrs->type_id     = WEAPON_BOW;
  attrs->rotate_hand = TRUE;

  ecs_add(registry, entity, HOLDER);

  ecs_set(registry,
          entity,
          WEAPON_SHOOT,
          &(WeaponShoot){
              .on_action = CHARACTER_ACTION_REGULAR_ATK,
              .projspd   = 170.f,
              .fire_rate = 20,
          });

  return entity;
}
