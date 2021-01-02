#include "components.h"
#include <stdlib.h>
#include <string.h>
#include <toolbox/sprite-sheet.h>

const EcsType g_comp_types[NUM_COMPONENTS] = {
  [VISUAL] =
      {
          .size = sizeof(Visual),
      },
  [TRANSFORM] =
      {
          .size = sizeof(Transform),
      },
  [ANIMATOR] =
      {
          .size    = sizeof(Animator),
          .fini_fn = (ecs_comp_fini_fn_t)animator_fini,
      },
  [PLAYER_TAG] =
      {
          .size = sizeof(PlayerTag),
      },
  [CONTROLLER] =
      {
          .size = sizeof(Controller),
      },
  [EQUIPMENT] =
      {
          .size = sizeof(Equipment),
      },
  [HEALTH] =
      {
          .size = sizeof(Health),
      },
  [HITBOX] =
      {
          .size    = sizeof(HitBox),
          .init_fn = (ecs_comp_init_fn_t)hitbox_init,
      },
  [ENEMY_TAG] =
      {
          .size = sizeof(EnemyTag),
      },
  [HEAL_BAR] =
      {
          .size = sizeof(HealthBar),
      },
  [LIFE_SPAN] =
      {
          .size = sizeof(LifeSpan),
      },
  [MOTION] =
      {
          .size = sizeof(Motion),
      },
  [PROJECTILE] =
      {
          .size = sizeof(Projectile),
      },
  [TAG_TO_BE_DESTROYED] =
      {
          .size = sizeof(TagToBeDestroyed),
      },
  [WEAPON_CORE] =
      {
          .size = sizeof(WeaponCore),
      },
  [DAMAGE_OUTPUT] =
      {
          .size = sizeof(DamageOutput),
      },
  [WEAPON_SKILL_SWING] =
      {
          .size = sizeof(wpskl_Swing),
      },
  [WEAPON_SKILL_CHARGE] =
      {
          .size = sizeof(wpskl_Charge),
      },
  [DROP] =
      {
          .size = sizeof(Drop),
      },
  [INVULNERABLE] =
      {
          .size = sizeof(Invulnerable),
      },
  [CAMERA_TARGET_TAG] =
      {
          .size = sizeof(CameraTargetTag),
      },
  [AI_AGENT] =
      {
          .size    = sizeof(AIAgent),
          .fini_fn = (ecs_comp_fini_fn_t)ai_agent_fini,
      },
  [DESTINATION] =
      {
          .size = sizeof(Destination),
      },
  [PATH] =
      {
          .size = sizeof(Path),
      },
  [FOLLOWING_TARGET] =
      {
          .size = sizeof(FollowingTarget),
      },
  [SPOT] =
      {
          .size = sizeof(Spot),
      },
  [WEAPON_SKILL_THUNDER_STORM] =
      {
          .size = sizeof(wpskl_ThunderStorm),
      },
  [LEVEL_SWITCHER] =
      {
          .size    = sizeof(LevelSwitcher),
          .fini_fn = (ecs_comp_fini_fn_t)level_switcher_fini,
      },
  [NAME] =
      {
          .size    = sizeof(Name),
          .fini_fn = (ecs_comp_fini_fn_t)name_fini,
      },
  [TEXT] =
      {
          .size    = sizeof(Text),
          .fini_fn = (ecs_comp_fini_fn_t)text_fini,
      },
  [INTERACTABLE] =
      {
          .size = sizeof(Interactable),
      },
  [DIALOGUE] =
      {
          .size = sizeof(Dialogue),
      },
  [ITEM_TAG] =
      {
          .size = sizeof(ItemTag),
      },
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
void interacable_set_cmd(Interactable* interactable, int index, const char* cmd)
{
  strncpy(interactable->text[index], cmd, 10);
}
