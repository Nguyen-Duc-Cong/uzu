#include "ecs/ecs.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "game_event/game_event.h"
#include "global.h"
#include "inventory.h"
#include "resources.h"
#include "session.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "ui_msgbox.h"
#include "ui_subtile.h"

#define SAV_FILE_NAME "event.begining"
extern Ecs* g_ecs;
enum
{
  PHASE_INITIAL,
  PHASE_1A,
  PHASE_2A,
};

static struct
{
  u8   phase;
  BOOL is_nova_alive;
} _save_block;

static Vec2 _npc_location_tbl[]     = { { 710.f, 90.f }, { 710.f, 90.f } };
static RECT _phase_1a_zone_to_clear = { 50, 31, 60, 217 };

SAVE_AND_LOAD_FN()
//*event callback*//
static void on_scene_unload(void* arg, const void* msg);
static void on_level_loaded(void* arg, const MSG_LevelLoaded* msg);
static void on_level_unload(void* arg, const MSG_LevelUnloaded* msg);
static void on_entity_died(void* arg, const MSG_EntityDied* msg);
static void phase_initial_on_conversation_finished(void* arg, const MSG_ConversationFinished* msg);
static void phase_1a_on_entity_died(void* arg, const MSG_EntityDied* msg);

static void init_default_data(void)
{
  _save_block.is_nova_alive = TRUE;
  _save_block.phase         = PHASE_INITIAL;
}

static void on_scene_unload(SDL_UNUSED void* arg, SDL_UNUSED const void* msg)
{
  write_data();
}

static void on_level_loaded(SDL_UNUSED void* arg, const MSG_LevelLoaded* msg)
{
  if (!_save_block.is_nova_alive)
    return;

  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);
  ems_connect(MSG_LEVEL_UNLOADED, NULL, on_level_unload);

  switch (_save_block.phase)
  {
  case PHASE_INITIAL:
    if (SDL_strcmp(msg->level_name, "demon_ruin") == 0)
    {
      make_npc_nova(g_ecs, _npc_location_tbl[PHASE_INITIAL], CONVERSATION_NOVA_00);
      ems_connect(MSG_CONVERSATION_FINISHED, NULL, phase_initial_on_conversation_finished);
    }
    break;
  case PHASE_1A:
    if (SDL_strcmp(msg->level_name, "demon_ruin") == 0)
    {
      make_npc_nova(g_ecs, _npc_location_tbl[PHASE_1A], CONVERSATION_NOVA_1A_1);
    }
    if (SDL_strcmp(msg->level_name, "corridor") == 0)
    {
      ems_connect(MSG_ENTITY_DIED, NULL, phase_1a_on_entity_died);
    }
    break;
  case PHASE_2A:
    if (SDL_strcmp(msg->level_name, "demon_ruin") == 0)
    {
      make_npc_nova(g_ecs, _npc_location_tbl[PHASE_1A], CONVERSATION_NOVA_2A_1);
    }
    break;
  }
}

static void on_level_unload(SDL_UNUSED void* arg, const MSG_LevelUnloaded* msg)
{
  ems_disconnect(MSG_ENTITY_DIED, (void*)on_entity_died);
  ems_disconnect(MSG_LEVEL_UNLOADED, (void*)on_level_unload);
  switch (_save_block.phase)
  {
  case PHASE_INITIAL:
    if (SDL_strcmp(msg->level_name, "demon_ruin") == 0)
    {
      ems_disconnect(MSG_CONVERSATION_FINISHED, (void*)phase_initial_on_conversation_finished);
    }
    break;
  case PHASE_1A:
    if (SDL_strcmp(msg->level_name, "corridor") == 0)
    {
      ems_disconnect(MSG_ENTITY_DIED, (void*)phase_1a_on_entity_died);
    }
  }
}

static void on_entity_died(SDL_UNUSED void* arg, const MSG_EntityDied* msg)
{
  Name* name;
  if ((name = ecs_get(g_ecs, msg->entity, NAME)) && SDL_strcmp(name->value, "nova") == 0)
  {
    _save_block.is_nova_alive = FALSE;
    ems_disconnect(MSG_ENTITY_DIED, (void*)on_entity_died);
  }
}

static void phase_initial_on_conversation_finished(SDL_UNUSED void*                arg,
                                                   const MSG_ConversationFinished* msg)
{
  Dialogue* dialogue;
  if (SDL_strcmp(msg->conversation_name, "nova_00") == 0)
  {
    ems_disconnect(MSG_CONVERSATION_FINISHED, (void*)phase_initial_on_conversation_finished);
    if (SDL_strcmp(msg->response, "accept his request") == 0)
    {
      _save_block.phase         = PHASE_1A;
      dialogue                  = ecs_get(g_ecs, msg->npc, DIALOGUE);
      dialogue->conversation_id = CONVERSATION_NOVA_1A_1;
    }
    else
    {
    }
  }
}

static void phase_1a_on_entity_died(SDL_UNUSED void* arg, const MSG_EntityDied* msg)
{
  extern struct Session g_session;
  ecs_size_t            enemy_count;

  ecs_raw(g_ecs, ENEMY_TAG, NULL, NULL, &enemy_count);
  if (ecs_has(g_ecs, msg->entity, ENEMY_TAG) && SDL_strcmp(g_session.level, "corridor") == 0 &&
      enemy_count <= 1)
  {
    _save_block.phase = PHASE_2A;
  }
}

void GE_begining_init(void)
{
  load_data();
  ems_connect(MSG_GAME_SCENE_UNLOAD, NULL, on_scene_unload);
  ems_connect(MSG_LEVEL_LOADED, NULL, on_level_loaded);
}
