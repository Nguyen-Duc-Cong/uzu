#ifndef ACTION_H
#define ACTION_H
#include "global.h"

typedef struct ActionVtbl
{
  u32 size;
  void (*start)(void* self, ecs_entity_t target);
  void (*end)(void* self, ecs_entity_t target);
  BOOL (*update)(void* self, ecs_entity_t target);
  void (*cleanup)(void* self);
} ActionVtbl;

typedef struct Action
{
  const ActionVtbl* vtbl;
} Action;

void* action_alloc(const ActionVtbl* type);
#define ACTION(ptr) ((Action*)ptr)
#define ACTION_START(action, target) ACTION(action)->vtbl->start(action, target)
#define ACTION_END(action, target) ACTION(action)->vtbl->end(action, target)
#define ACTION_UPDATE(action, target) ACTION(action)->vtbl->update(action, target)

void action_default_start_func(void* self, ecs_entity_t target);
void action_default_end_func(void* self, ecs_entity_t target);
BOOL action_default_update_func(void* self, ecs_entity_t target);
void action_default_cleanup_func(void* self);
void action_delete(Action* action);

typedef struct CompositeAction
{
  Action    parent_instance;
  PtrArray* children;
} CompositeAction;

#define COMPOSITE_ACTION(ptr) ((CompositeAction*)ptr)

CompositeAction* composite_action_init(CompositeAction* self);
void             composite_action_cleanup(void* self);
void             composite_action_add(CompositeAction* self, Action* action);
void             composite_action_addn(CompositeAction* self, int n, ...);

Action* parallel_action_new(void);
Action* sequence_action_new(void);
Action* walk_action_new(Vec2 destination);
Action* talk_action_new(u16 conversation_id);
Action* express_emotion_action_new(int emoji, int duration);
Action* wait_action_new(int duration);
#endif // ACTION_H
