#include "system/weapon_skill/swing.h"
#include "entity_factory.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"
#include <components.h>

#define SWINGING_SYSTEM_STEP 8
#define SWINGING_SYSTEM_INTERVAL 1
#define SWINGING_SYSTEM_DAMAGE_ADJ 1.0
extern Ecs* g_ecs;

typedef struct CBSwing_QueryHitEntitiesArgs
{
  ecs_entity_t dealer;
  int          damage;
  Vec2         force;
} CBSwing_QueryHitEntitiesArgs;

const static Vec2 k_impact_force = { 50.f, 0.f };

static BOOL __cb_query_hit_entities(CBSwing_QueryHitEntitiesArgs* args, ecs_entity_t e)
{

  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){ .dealer      = args->dealer,
                                   .receiver    = e,
                                   .damage      = args->damage,
                                   .impact      = TRUE,
                                   .impact_time = 10,
                                   .force       = args->force });
  return TRUE;
}

void swing_weapon_skl_system_update(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  wpskl_Swing*      skl;
  WeaponAttributes* attributes;
  Transform*        transform;
  Controller*       holder_controller;
  Holder*           holder;
  RECT              damage_area;

  ecs_raw(g_ecs, WEAPON_SKILL_SWING, &entities, (void**)&skl, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    switch (skl[i].state)
    {
    case 0:
      if ((holder = ecs_get(g_ecs, entities[i], HOLDER)) &&
          (holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)) &&
          (transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
      {
        if (!holder_controller->in_action && holder_controller->action == skl[i].on_action)
        {
          skl[i].timer                 = 5;
          skl[i].state                 = 1;
          holder_controller->in_action = TRUE;
          holder_controller->action    = CHARACTER_ACTION_NONE;
          transform->rotation          = -15 * transform->hdir;
        }
      }
      break;
    case 1:
      if (skl[i].timer > 0 && --skl[i].timer == 0)
      {
        transform           = ecs_get(g_ecs, entities[i], TRANSFORM);
        skl[i].state        = 2;
        skl[i].timer        = 7;
        transform->rotation = 115 * transform->hdir;

        SDL_RendererFlip flip = transform->hdir < 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        Vec2 particle_position = { transform->position.x + skl[i].range / 2.f * transform->hdir,
                                   transform->position.y - 13};
        make_slash_effect(g_ecs, particle_position, flip);
        Mix_PlayChannel(-1, get_sfx(SFX_WEAPON_SWORD), 0);
      }
      break;
    case 2:
      if (skl[i].timer > 0 && --skl[i].timer == 0)
      {
        transform         = ecs_get(g_ecs, entities[i], TRANSFORM);
        attributes        = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES);
        holder            = ecs_get(g_ecs, entities[i], HOLDER);
        holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER);

        transform->rotation          = 0.0;
        skl[i].state                 = 0;
        holder_controller->in_action = FALSE;

        damage_area.w = skl[i].range;
        damage_area.h = 32;
        damage_area.y = transform->position.y - 16;
        damage_area.x =
            transform->hdir > 0 ? transform->position.x : transform->position.x - damage_area.w;
        Vec2 force = vec2_mul(k_impact_force, transform->hdir);
        collision_box_query(&damage_area,
                            attributes->mask,
                            CALLBACK_1((&(CBSwing_QueryHitEntitiesArgs){
                                           .damage = attributes->atk,
                                           .force  = force,
                                           .dealer = holder->value,
                                       }),
                                       __cb_query_hit_entities));
      }
    }
  }
}
