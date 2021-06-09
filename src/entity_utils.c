#include "entity_utils.h"
#include "components.h"
ecs_entity_t scn_get_player(ecs_Registry* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_raw(ecs, PLAYER_TAG, &entities, NULL, &cnt);
  return cnt > 0 ? entities[0] : ECS_NULL_ENT;
}

Vec2 snn_get_player_position(ecs_Registry* ecs)
{
  return ett_get_position(ecs, scn_get_player(ecs));
}

Vec2 ett_get_position(ecs_Registry* ecs, ecs_entity_t entity)
{
  return ((Transform*)ecs_get(ecs, entity, TRANSFORM))->position;
}

void ett_set_position(ecs_Registry* ecs, ecs_entity_t entity, Vec2 pos)
{
  Transform* transform = ecs_get(ecs, entity, TRANSFORM);
  transform->position  = pos;
}

void ett_set_velocity(ecs_Registry* ecs, ecs_entity_t entity, Vec2 vel)
{
  Motion* motion = ecs_get(ecs, entity, MOTION);
  motion->vel    = vel;
}

ecs_entity_t scn_find_portal(ecs_Registry* ecs, const char* _name)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  PortalAttributes* portal_attributes;
  Name*             name;

  ecs_raw(ecs, LADDER_ATTRIBUTES, &entities, (pointer_t*)&portal_attributes, &cnt);
  for (int i = 0; i < cnt; ++i)
    if ((name = ecs_get(ecs, entities[i], NAME)) && !SDL_strcmp(_name, name->value))
      return entities[i];
  return ECS_NULL_ENT;
}

void ett_equip_weapon(ecs_Registry* ecs, ecs_entity_t entity, ecs_entity_t weapon)
{
  Hand* equipment;

  ASSERT(entity != ECS_NULL_ENT && weapon != ECS_NULL_ENT);
  equipment = ecs_get(ecs, entity, HAND);

  ecs_set(ecs, weapon, HOLDER, &(Holder){ .value = entity });
  if (equipment->weapon != ECS_NULL_ENT)
    ecs_add(ecs, equipment->weapon, DESTROYED_TAG);
  equipment->weapon = weapon;
}

ecs_entity_t ett_get_equiped_weapon(ecs_Registry* ecs, ecs_entity_t entity)
{
  Hand* hand = ecs_get(ecs, entity, HAND);
  return hand != NULL ? hand->weapon : ECS_NULL_ENT;
}

ecs_entity_t scn_find(ecs_Registry* ecs, const char* name)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Name*         names;

  ecs_raw(ecs, NAME, &entities, (pointer_t*)&names, &cnt);
  for (int i = 0; i < cnt; ++i)
    if (!SDL_strcmp(names[i].value, name))
      return entities[i];
  return ECS_NULL_ENT;
}

float ett_dist(ecs_Registry* ecs, ecs_entity_t e1, ecs_entity_t e2)
{
  Vec2 p1, p2;
  p1 = ett_get_position(ecs, e1);
  p2 = ett_get_position(ecs, e2);
  return vec2_mag(vec2_sub(p1, p2));
}

void ett_set_hp(ecs_Registry* ecs, ecs_entity_t entity, u16 hit_points)
{
  ((Health*)ecs_get(ecs, entity, HEALTH))->current = hit_points;
}

u16 ett_get_hp(ecs_Registry* ecs, ecs_entity_t entity)
{
  return ((Health*)ecs_get(ecs, entity, HEALTH))->current;
}

const Conversation* ett_get_conversation(ecs_Registry* ecs, ecs_entity_t entity)
{
  return ((Dialogue*)ecs_get(ecs, entity, DIALOGUE))->conversation;
}

const char* ett_get_name(ecs_Registry* ecs, ecs_entity_t entity)
{
  return ((Name*)ecs_get(ecs, entity, NAME))->value;
}

Vec2 ett_get_velocity(ecs_Registry* ecs, ecs_entity_t entity)
{
  return ((Motion*)ecs_get(ecs, entity, MOTION))->vel;
}

u16 ett_get_mp(ecs_Registry* ecs, ecs_entity_t entity)
{
  return ((Mana*)ecs_get(ecs, entity, MANA))->current;
}

void ett_set_mp(ecs_Registry* ecs, ecs_entity_t entity, u16 current)
{
  ((Mana*)ecs_get(ecs, entity, MANA))->current = current;
}
void ett_attune_spell(ecs_Registry* ecs, ecs_entity_t entity, u16 spell_id)
{
  ecs_set(ecs, entity, ATTUNEMENT_SLOT, &(AttunementSlot){ spell_id });
}
u16 ett_get_attuned_spell_type(ecs_Registry* ecs, ecs_entity_t entity)
{
  AttunementSlot* comp = ecs_get(ecs, entity, ATTUNEMENT_SLOT);
  return comp ? comp->spellId : SPELL_ID_NULL;
}

const Spell* ett_get_attuned_spell(ecs_Registry* registry, ecs_entity_t entity)
{
  u16 id = ett_get_attuned_spell_type(registry, entity);
  return id != SPELL_ID_NULL ? &gSpellTbl[id] : NULL;
}
u16 wpn_get_type(ecs_Registry* ecs, ecs_entity_t weapon)
{
  return ((const WeaponAttributes*)ecs_get(ecs, weapon, WEAPON_ATTRIBUTES))->typeId;
}
ecs_entity_t get_equiped_weapon(ecs_Registry* registry, ecs_entity_t holder)
{
  return ((const Hand*)ecs_get(registry, holder, HAND))->weapon;
}
u16 ett_get_equiped_weapon_type(ecs_Registry* ecs, ecs_entity_t holder)
{
  ecs_entity_t weapon = get_equiped_weapon(ecs, holder);
  return weapon != ECS_NULL_ENT ? wpn_get_type(ecs, weapon) : WEAPON_ID_NULL;
}

void ett_set_conversation(ecs_Registry* ecs, ecs_entity_t entity, u16 conversation_id)
{
  ecs_set(ecs, entity, DIALOGUE, &(Dialogue){ conversation_id });
}

void ett_animate_hand(ecs_Registry* registry, ecs_entity_t entity, const HandAnimParams* params)
{
  Hand*          hand      = ecs_get(registry, entity, HAND);
  HandAnimation* ha        = ecs_add(registry, entity, HAND_ANIMATION);
  ha->currentIndex         = -1;
  ha->initialAngle         = hand->angle;
  ha->initialOriginalPoint = hand->originalPoint;
  ha->initialLength        = hand->length;
  ha->keyframes            = params->keyframes;
  ha->relative             = params->realtiveCurrentState;
  ha->cbCompleted          = params->cbFinished;
  ha->cbFrame              = params->cbFrame;
}

Vec2 ett_get_facing_direction(ecs_Registry* registry, ecs_entity_t entity)
{
  return ((AimDirection*)ecs_get(registry, entity, AIM_DIRECTION))->value;
}

u16 ett_get_atk_mask(ecs_Registry* registry, ecs_entity_t entity)
{
  return ((AttackMask*)ecs_get(registry, entity, ATTACK_MASK))->value;
}

ecs_entity_t wpn_get_holder(ecs_Registry* registry, ecs_entity_t weapon)
{
  Holder* holder = ecs_get(registry, weapon, HOLDER);
  return holder ? holder->value : ECS_NULL_ENT;
}
void ett_unable_to_move_push(ecs_Registry* registry, ecs_entity_t entity)
{
  UnableToMove* unable_to_move = ecs_get(registry, entity, UNABLE_TO_MOVE);
  if (unable_to_move)
    ++unable_to_move->count;
  else
    ecs_set(registry, entity, UNABLE_TO_MOVE, &(UnableToMove){ 1 });
}
void ett_unable_to_move_pop(ecs_Registry* registry, ecs_entity_t entity)
{
  UnableToMove* unable_to_move = ecs_get(registry, entity, UNABLE_TO_MOVE);
  if (unable_to_move && --unable_to_move->count == 0)
    ecs_rmv(registry, entity, UNABLE_TO_MOVE);
}
void ett_apply_status_effect(ecs_Registry* registry, ecs_entity_t entity, u16 type, u16 duration)
{
  if (!ecs_has(registry, entity, STATUS_EFFECT))
    ecs_set(registry, entity, STATUS_EFFECT, &(StatusEffect){ .type = type, .duration = duration });
}
