#include "enemy_ai_system.h"
#include <components.h>
#include <stdio.h>
#include <stdlib.h>
void EnemyAiSystem(Ecs* ecs)
{
  ecs_entity_t* player;
  hitbox*       pHitbox;
  esc_data(ecs, PLAYER_TAG, &player, (void**)&hitbox, null);
  Transform* plx = ecs_get(ecs, player, TRANSFORM);

  ecs_entity_t* enemy;
  ecs_size_t    cnt;
  hitbox*       eHisboxs;
  ecs_data(ecs, ENEMY_TAG, &enemy, (void**)&ehitboxs, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    Transform* dlx = ecs_get(ecs, [i], TRANSFORM);
    u16        x   = plx->pos.x - dlx->pos.x;
    u16        y   = plx->pos.y - dlx->pos.y;
  }
}

enum
{
  RUNNING,
  SUCCESS,
  FAILURE
} NodeState;

typedef struct Node
{
  int          data;
  struct Node* next;
  NodeState    state;
} Node;

struct bh_tree
{
  int category;
};

void BehaviorTree()
{

  Node* nodes;

  NodeState Sequence()
  {
    Node node;
    bool is_any_node_running = FALSE;
    do
    {
      switch (node->state)
      {
      case NodeState.RUNNING:
        is_any_node_running = TRUE;
        break;
      case NodeState.SUCCESS:
        break;
      case NodeState.FAILURE:
        return NodeState.FAILURE;
      default:
        break;
      }
    } while (node->next == NULL);
    return is_any_node_running == TRUE ? NodeState.RUNNING : NodeState.SUCCESS;
  }
  void selector()
  {
    Node node;
    bool is_any_node_running = FALSE;
    do
    {
      switch (node->state)
      {
      case NodeState.RUNNING:
        return NodeState.RUNNING;
      case NodeState.SUCCESS:
        return NodeState.SUCCESS;
      case NodeState.FAILURE:
        break;
      default:
        break;
      }
    } while (node->next == NULL);
    return NodeState.FAILURE;
  }

  void Inverter()
  {
    Node node;

    switch (node->state)
    {
    case NodeState.RUNNING:
      return NodeState.RUNNING;
    case NodeState.SUCCESS:
      return NodeState.SUCCESS;
    case NodeState.FAILURE:
      break;
    default:
      break;
    }
    return NodeState.FAILURE;
  }
}

void HealthNode()
{


