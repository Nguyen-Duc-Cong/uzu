#include "ai/is_player_out_of_spot.h"
#include <components.h>
#include "../../include/entity_utils.h"

const BTNodeVtbl* is_player_out_of_spot_vtbl_inst();
void              is_player_out_of_spot_vtbl_init(BTConditionVtbl* vtbl);

BTCondition_IsPlayerOutOfSpot* is_player_out_of_spot_init(BTCondition_IsPlayerOutOfSpot* self);
BOOL is_player_out_of_spot_pred(BTCondition_IsPlayerOutOfSpot* self, Ecs* ecs, ecs_entity_t entity);

BT_ALLOC_FN(BTCondition_IsPlayerOutOfSpot, is_player_out_of_spot)
BT_VTBL_INST_FN(BTCondition, is_player_out_of_spot)

void is_player_out_of_spot_vtbl_init(BTConditionVtbl* vtbl)
{
  bt_condition_vtbl_init(vtbl);
  ((BTNodeVtbl*)vtbl)->parent = bt_condition_vtbl_inst();
  vtbl->pred                  = (bt_pred_fn_t)is_player_out_of_spot_pred;
}

BTCondition_IsPlayerOutOfSpot* bt_condition_is_player_out_of_spot_new()
{
  return is_player_out_of_spot_init(is_player_out_of_spot_alloc());
}

BTCondition_IsPlayerOutOfSpot* is_player_out_of_spot_init(BTCondition_IsPlayerOutOfSpot* self)
{
  bt_condition_init((BTCondition*)self, TRUE);
  return self;
}

BOOL is_player_out_of_spot_pred(BTCondition_IsPlayerOutOfSpot* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  ecs_entity_t player;
  Transform*   player_transform;
  Spot*        spot;

  if ((player = get_player(ecs)) == ECS_NULL_ENT)
    return FALSE;

  player_transform = ecs_get(ecs, player, TRANSFORM);
  spot             = ecs_get(ecs, entity, SPOT);

  ASSERT(spot != NULL);
  ASSERT(player_transform != NULL);

  return vec2_dist(player_transform->position, spot->position) > spot->radius;
}
