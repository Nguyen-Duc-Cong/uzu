#include "life_span_sys.h"

#include <components.h>
#include "event_messaging_sys.h"

extern Ecs* g_ecs;

void life_span_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  LifeSpan*     life_span;

  ecs_raw(g_ecs, LIFE_SPAN, &entities, (void**)&life_span, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (--life_span[i].remaining == 0)
    {
      INFO("e"ECS_ENT_FMT_PATTERN" finished life span\n", ECS_ENT_FMT_VARS(entities[i]));
      ems_broadcast(MSG_LIFE_SPAN_FINISHED, &(MSG_LifeSpanFinished){ entities[i] });
      ecs_destroy(g_ecs, entities[i]);
    }
  }
}
