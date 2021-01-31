#include "attack.h"
#include "utils.h"
#include <components.h>
#include <constances.h>

struct Attack
{
  BT_EXTEND_NODE(BTNode)
  BOOL is_running;
};

static void     attack_vtbl_init(BTNodeVtbl* vtbl);
static Attack*  attack_init(Attack* self);
static void     attack_finish(Attack* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
static void     attack_abort(Attack* self, Ecs* ecs, ecs_entity_t entity);
static BTStatus attack_exec(Attack* self, Ecs* ecs, ecs_entity_t entity);

BT_VTBL_INST_FN(BTNode, attack)
BT_ALLOC_FN(Attack, attack)

static void attack_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)attack_exec;
  vtbl->abort  = (bt_abort_fn_t)attack_abort;
  vtbl->finish = (bt_finish_fn_t)attack_finish;
}

Attack* attack_new()
{
  return attack_init(attack_alloc());
}

static Attack* attack_init(Attack* self)
{
  bt_node_init((BTNode*)self);
  self->is_running = FALSE;
  return self;
}

static void attack_abort(Attack* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  self->is_running = FALSE;
}

static void attack_finish(Attack* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->is_running = FALSE;
}

static BTStatus attack_exec(Attack* self, Ecs* ecs, ecs_entity_t entity)
{
  Controller*  controller;
  ecs_entity_t player;
  Vec2         entity_position, player_position;
  Transform*   transform;

  controller = ecs_get(ecs, entity, CONTROLLER);

  if (!self->is_running)
  {
    player = get_player(ecs);
    if (controller->in_action || ecs_has(ecs, entity, INPUT_BLOCKER) || player == ECS_NULL_ENT)
      return BT_STATUS_FAILURE;
    transform       = ecs_get(ecs, entity, TRANSFORM);
    entity_position = transform->position;
    player_position = get_entity_position(ecs, player);

    transform->hdir = (entity_position.x < player_position.x) ? 1 : -1;

    controller->action = CHARACTER_ACTION_REGULAR_ATK;
    self->is_running   = TRUE;
    return BT_STATUS_RUNNING;
  }
  else
  {
    if (controller->in_action)
      return BT_STATUS_RUNNING;
    else
    {
      self->is_running = FALSE;
      return BT_STATUS_SUCCESS;
    }
  }
}
