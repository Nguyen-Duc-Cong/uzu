
#include "bttask.h"
#include "components.h"
#include "constances.h"
#include "entity_utils.h"

struct BTTask_Attack
{
  BT_EXTEND_NODE(BTNode)
  BOOL is_running;
};

#define NODE BTTask_Attack

static void     __vtbl_init(BTNodeVtbl* vtbl);
static NODE*    __init(BTTask_Attack* self);
static void     __finish(NODE* self, Ecs* ecs, ecs_entity_t entity, BOOL succeed);
static void     __abort(NODE* self, Ecs* ecs, ecs_entity_t entity);
static BTStatus __exec(NODE* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(NODE, _)

static void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

NODE* bt_task_attack_new()
{
  return __init(__alloc());
}

static NODE* __init(BTTask_Attack* self)
{
  bt_node_init((BTNode*)self);
  self->is_running = FALSE;
  return self;
}

static void __abort(NODE* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  self->is_running = FALSE;
}

static void
__finish(NODE* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity, SDL_UNUSED BOOL succeed)
{
  self->is_running = FALSE;
}

static BTStatus __exec(NODE* self, Ecs* ecs, ecs_entity_t entity)
{
  Controller*   controller;
  Hand*         hand;
  AttackTarget* attack_target;
  BOOL          has_attack_target;

  controller    = ecs_get(ecs, entity, CONTROLLER);
  attack_target = ecs_get(ecs, entity, ATTACK_TARGET);

  has_attack_target = attack_target != NULL && attack_target->value != ECS_NULL_ENT;

  if (!self->is_running)
  {
    hand = ecs_get(ecs, entity, HAND);
    if (controller->in_action || ecs_has(ecs, entity, PARALYZED) || !has_attack_target ||
        hand->weapon == ECS_NULL_ENT)
      return BT_STATUS_FAILURE;

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
      return BT_STATUS_SUCCESS;
    }
  }
}

#undef NODE
