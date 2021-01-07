#include "../mediator.h"
#include "components.h"
#include "game_event.h"
#include <entity_factory.h>
#include <utils.h>

extern Ecs* g_ecs;

//<--------------------------------------------------------------------------------->//
static void on_finish_conversation(pointer_t arg, const SysEvt_ConversationFinished* event);
//<===========================================================================>//

void GE_get_weapon_init(void)
{
  mediator_connect(SYS_SIG_CONVERSATION_FINISHED, NULL, SLOT(on_finish_conversation));
}

static void on_finish_conversation(pointer_t arg, const SysEvt_ConversationFinished* event)
{
  (void)arg;
  if (strcmp(event->conversation_name, "demo1") == 0 && strcmp(event->response, "yes") == 0)
  {

    // thay đổi lời thoại của NPC
    Dialogue* dialogue;

    dialogue                  = ecs_get(g_ecs, event->npc, DIALOGUE);
    dialogue->conversation_id = CONVERSATION_DEMO2;

    //đưa vũ khí cho player

    ecs_entity_t weapon = make_golden_sword(g_ecs, BIT(CATEGORY_ENEMY));
    equip(g_ecs, get_player(g_ecs), weapon);

    mediator_disconnect(SYS_SIG_CONVERSATION_FINISHED, (pointer_t)on_finish_conversation);
    mediator_broadcast(SYS_SIG_EVENT_FINISHED, &(SysEvt_EventFinished){ GAME_EVENT_GET_WEAPON });
  }
}
