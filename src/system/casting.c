#include "components.h"
#include "ecs/ecs.h"
#include "entity_utils.h"
#include "global.h"

extern const Spell gSpellTbl[];
extern ecs_Registry*        gRegistry;

const HandAnimKeyFrame _keyframes[] = {
  { .duration = 2, .length = -1, .angle = -15.0 },
  { .duration = 2, .length = -2, .angle = -20.0 },
  { .duration = 2, .length = 0, .angle = -20.0 },
  { .duration = 3, .length = 3, .angle = 0 },
  { .duration = -1 },
};

void weapon_casting_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponCast*    cast;
  Mana*          mana;
  const Spell*   spell;
  BOOL           isTriggered;
  BOOL           hasEnoughMp;
  ecs_entity_t   holder;
  AttackCommand* attackCommand;

  ecs_raw(gRegistry, WEAPON_CAST, &entities, (pointer_t*)&cast, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (cast[i].processing)
    {
      holder        = wpn_get_holder(gRegistry, entities[i]);
      spell         = ett_get_attuned_spell(gRegistry, holder);
      attackCommand = ecs_get(gRegistry, holder, ATTACK_COMMAND);
      if (spell->process(gRegistry, holder, entities[i]))
      {
        cast->processing = FALSE;
        INVOKE_EVENT(attackCommand->cbCompleted, TRUE);
        ecs_rmv(gRegistry, holder, ATTACK_COMMAND);
      }
    }
    else if (cast[i].cooldownTimer == 0 &&
             ((holder = wpn_get_holder(gRegistry, entities[i])) != ECS_NULL_ENT) &&
             (attackCommand = ecs_get(gRegistry, holder, ATTACK_COMMAND)))
    {
      spell = ett_get_attuned_spell(gRegistry, holder);
      mana  = ecs_get(gRegistry, holder, MANA);

      isTriggered = !attackCommand->processing && attackCommand->triggerCode == 0;
      hasEnoughMp = mana->current >= spell->cost;

      if (isTriggered)
      {
        if (hasEnoughMp)
        {
          mana->current -= spell->cost;
          spell->cast(gRegistry, holder, entities[i]);
          cast[i].processing        = TRUE;
          cast[i].cooldownTimer     = spell->coolDownTime;
          attackCommand->processing = TRUE;

          HandAnimParams params       = { 0 };
          params.keyframes            = _keyframes;
          params.realtiveCurrentState = TRUE;
          ett_animate_hand(gRegistry, holder, &params);
        }
        else
        {
          INVOKE_EVENT(attackCommand->cbCompleted, FALSE);
          ecs_rmv(gRegistry, holder, ATTACK_COMMAND);
        }
      }
    }
    else if (cast[i].cooldownTimer > 0)
    {
      --cast[i].cooldownTimer;
    }
  }
}
