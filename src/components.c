#include "components.h"
#include <stdlib.h>
#include <string.h>
#include <toolbox/sprite-sheet.h>

#define ECS_TYPE(__T)                                                                              \
  {                                                                                                \
    .size = sizeof(__T)                                                                            \
  }
#define ECS_TYPE_EX(__T, __init_fn, __fini_fn)                                                     \
  {                                                                                                \
    .size = sizeof(__T), .init_fn = (ecs_comp_init_fn_t)__init_fn,                                 \
    .fini_fn = (ecs_comp_fini_fn_t)__fini_fn                                                       \
  }

const EcsType g_comp_types[NUM_COMPONENTS] = {
  [VISUAL]                     = ECS_TYPE(Visual),
  [TRANSFORM]                  = ECS_TYPE(Transform),
  [ANIMATOR]                   = ECS_TYPE_EX(Animator, NULL, animator_fini),
  [PLAYER_TAG]                 = ECS_TYPE(PlayerTag),
  [CONTROLLER]                 = ECS_TYPE(Controller),
  [EQUIPMENT]                  = ECS_TYPE(Equipment),
  [HEALTH]                     = ECS_TYPE(Health),
  [HITBOX]                     = ECS_TYPE_EX(HitBox, hitbox_init, NULL),
  [ENEMY_TAG]                  = ECS_TYPE(PlayerTag),
  [HEAL_BAR]                   = ECS_TYPE(HealthBar),
  [LIFE_SPAN]                  = ECS_TYPE(LifeSpan),
  [MOTION]                     = ECS_TYPE(Motion),
  [PROJECTILE]                 = ECS_TYPE(Projectile),
  [TAG_TO_BE_DESTROYED]        = ECS_TYPE(TagToBeDestroyed),
  [WEAPON_CORE]                = ECS_TYPE(WeaponCore),
  [DAMAGE_OUTPUT]              = ECS_TYPE(DamageOutput),
  [WEAPON_SKILL_SWING]         = ECS_TYPE(wpskl_Swing),
  [WEAPON_SKILL_CHARGE]        = ECS_TYPE(wpskl_Charge),
  [DROP]                       = ECS_TYPE(Drop),
  [INVULNERABLE]               = ECS_TYPE(Invulnerable),
  [CAMERA_TARGET_TAG]          = ECS_TYPE(CameraTargetTag),
  [AI_AGENT]                   = ECS_TYPE_EX(AIAgent, NULL, ai_agent_fini),
  [DESTINATION]                = ECS_TYPE(Destination),
  [PATH]                       = ECS_TYPE(Path),
  [FOLLOWING_TARGET]           = ECS_TYPE(FollowingTarget),
  [SPOT]                       = ECS_TYPE(Spot),
  [WEAPON_SKILL_THUNDER_STORM] = ECS_TYPE(wpskl_ThunderStorm),
  [LEVEL_SWITCHER]             = ECS_TYPE_EX(LevelSwitcher, NULL, level_switcher_fini),
  [NAME]                       = { .size = sizeof(Name), .fini_fn = (ecs_comp_fini_fn_t)name_fini },
  [TEXT]                       = { .size = sizeof(Text), .fini_fn = (ecs_comp_fini_fn_t)text_fini },
  [INTERACTABLE]               = { .size = sizeof(Interactable) },
  [DIALOGUE]                   = { .size = sizeof(Dialogue) },
  [ITEM_TAG]                   = { .size = sizeof(ItemTag) },
  [MERCHANT]                   = { .size = sizeof(Merchant) }
};

Animation*
animation_init(Animation* anim, SDL_Texture* tex, u32 x, u32 y, u32 row, u32 col, u32 sw, u32 sh)
{
  sprite_sheet_init_1(&anim->sheet, tex, x, y, sw, sh, row, col);
  anim->frame_duration = 1;
  return anim;
}

void animation_fini(Animation* anim)
{
  sprite_sheet_fini(&anim->sheet);
}

Animator* animator_init(Animator* animator, const Animation* anims, u32 cnt)
{
  animator->anims        = malloc(cnt * sizeof(Animation));
  animator->cnt          = cnt;
  animator->elapsed      = 0;
  animator->current_anim = 0;
  memmove(animator->anims, anims, cnt * sizeof(Animation));
  return animator;
}

void animator_fini(Animator* animator)
{
  for (u32 i = 0; i < animator->cnt; ++i)
  {
    animation_fini(&animator->anims[i]);
  }
  free(animator->anims);
}

void visual_set_anchor_to_center(Visual* v)
{
  v->anchor.x = v->sprite.rect.w / 2;
  v->anchor.y = v->sprite.rect.h / 2;
}

void ai_agent_fini(AIAgent* ai_agent)
{
  bt_node_del(ai_agent->root);
  ai_agent->root = NULL;
}

void hitbox_init(HitBox* h)
{
  h->proxy_id = RTREE_NULL_NODE;
}

void level_switcher_init(LevelSwitcher* sw, const char* level, const char* dest)
{
  sw->level = strdup(level);
  sw->dest  = strdup(dest);
}

void level_switcher_fini(LevelSwitcher* sw)
{
  free(sw->level);
  free(sw->dest);
}

void name_fini(Name* name)
{
  free(name->value);
}

void name_init(Name* name, const char* value)
{
  name->value = strdup(value);
}

void text_init(Text* text, const char* value, FONT* font, COLOR color)
{
  text->value   = strdup(value);
  text->opacity = 0xff;
  text->color   = color;
  text->font    = font;
}

void text_fini(Text* text)
{
  free(text->value);
}
