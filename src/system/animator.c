#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void animator_system(void)
{
  ecs_entity_t* entities;
  Animator*     animator;
  Visual*       visual;
  ecs_size_t    cnt;

  const Animation*         curr;
  int                      idx;
  RECT                     rect;

  ecs_raw(g_ecs, ANIMATOR, &entities, (void**)&animator, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    visual = ecs_get(g_ecs, entities[i], VISUAL);
    curr   = &animator[i].anims[animator[i].currentAnimation];

    idx = animator[i].elapsed / curr->frame_duration;
    idx = idx % curr->count;

    rect.x = curr->offset_x + curr->sprite_width * idx;
    rect.y = curr->offset_y;
    rect.w = curr->sprite_width;
    rect.h = curr->sprite_height;

    visual->sprite.texture_id = curr->texture_id;
    visual->sprite.rect       = rect;

    animator[i].elapsed++;
  }
}
