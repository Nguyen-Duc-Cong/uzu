#include "components.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"

extern ecs_Registry* gRegistry;

static HandAnimKeyFrame _keyframes[] = {
  { .duration = 5, .angle = -60.0 },
  { .duration = 7, .angle = 60.0 },
  { .duration = -1 },
};

struct _QueryOverlapCallbackData
{
  ecs_entity_t entity;
  int          damage;
  Vec2         impact_force;
};

static void query_overlap_callback(struct _QueryOverlapCallbackData* d, ecs_entity_t e)
{
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(InflictDamageMsg){
                    .dealer     = d->entity,
                    .receiver   = e,
                    .damage     = d->damage,
                    .type       = DAMAGE_TYPE_STRIKE,
                    .impactTime = 10,
                    .force      = d->impact_force,
                });
}

static void hand_animation_finished_callback(SDL_UNUSED void* arg, ecs_entity_t entity)
{
  HandAnimation* hanim         = ecs_get(gRegistry, entity, HAND_ANIMATION);
  Hand*          hand          = ecs_get(gRegistry, entity, HAND);
  AttackMask*    mask          = ecs_get(gRegistry, entity, ATTACK_MASK);
  Transform*     transform     = ecs_get(gRegistry, entity, TRANSFORM);
  AimDirection*  aimDirection  = ecs_get(gRegistry, entity, AIM_DIRECTION);
  AttackCommand* attackCommand = ecs_get(gRegistry, entity, ATTACK_COMMAND);

  WeaponAttributes*  weaponAttributes = ecs_get(gRegistry, hand->weapon, WEAPON_ATTRIBUTES);
  WeaponSwingAttack* swing            = ecs_get(gRegistry, hand->weapon, WEAPON_SWING_ATTACK);

  INVOKE_EVENT(attackCommand->cbCompleted, TRUE);
  ecs_rmv(gRegistry, entity, ATTACK_COMMAND);
  double a        = hanim->initialAngle * DEG_TO_RAD;
  int    midRange = (weaponAttributes->range.min + weaponAttributes->range.max) / 2;

  Vec2 v;
  v.x = SDL_cos(a) * midRange;
  v.y = SDL_sin(a) * midRange;
  v   = vec2_add(v, transform->position);
  v   = vec2_add(v, hand->originalPoint);

  RECT box;
  box.x = v.x - swing->wide / 2;
  box.y = v.y - swing->wide / 2;
  box.w = box.h = swing->wide;

  struct _QueryOverlapCallbackData cbdata = { entity,
                                              weaponAttributes->atk,
                                              vec2_mul(aimDirection->value, 60.f) };
  collision_box_query(&box, mask->value, CALLBACK_1(&cbdata, query_overlap_callback));
}

static void play_slash_effect(ecs_Registry* registry, ecs_entity_t entity, float length)
{
  Vec2             pos;
  double           rot;
  SDL_RendererFlip flip;
  Transform*       transform;
  Hand*            hand;
  AimDirection*    facingDirection;

  transform       = ecs_get(registry, entity, TRANSFORM);
  hand            = ecs_get(registry, entity, HAND);
  facingDirection = ecs_get(registry, entity, AIM_DIRECTION);

  pos = transform->position;
  pos = vec2_add(pos, hand->originalPoint);
  pos = vec2_add(pos, vec2_mul(facingDirection->value, length));

  rot = SDL_atan2f(facingDirection->value.y, facingDirection->value.x) * RAD_TO_DEG;
  rot += signf(facingDirection->value.x) * 45.0;

  flip = facingDirection->value.x > 0.f ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;

  make_fx_slash(registry, pos, rot, flip);
}

void weapon_swing_attack_system(void)
{
  ecs_entity_t*      entities;
  ecs_size_t         cnt;
  WeaponSwingAttack* swing;
  Holder*            holder;
  AttackCommand*     attackCommand;

  ecs_raw(gRegistry, WEAPON_SWING_ATTACK, &entities, (void**)&swing, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    holder = ecs_get(gRegistry, entities[i], HOLDER);
    if (holder == NULL || !ecs_is_valid(gRegistry, holder->value))
      continue;

    attackCommand = ecs_get(gRegistry, holder->value, ATTACK_COMMAND);
    if (attackCommand == NULL || attackCommand->processing)
      continue;

    if (attackCommand->triggerCode == swing[i].code &&
        !ecs_has(gRegistry, holder->value, HAND_ANIMATION))
    {
      HandAnimParams params       = { 0 };
      params.cbFinished           = CALLBACK_2(hand_animation_finished_callback);
      params.keyframes            = _keyframes;
      params.realtiveCurrentState = TRUE;
      ett_animate_hand(gRegistry, holder->value, &params);
      attackCommand->processing = TRUE;
      play_slash_effect(gRegistry, holder->value, 16);
      Mix_PlayChannel(-1, get_sfx(SFX_WEAPON_SWORD), 0);
    }
  }
}
