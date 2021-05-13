#include "action.h"
#include "config.h"

void* _action_alloc(size_t size, const ActionVtbl* type)
{
  Action* action = SDL_malloc(size);
  action->vtbl   = type;
  return action;
}

void action_delete(Action* action)
{
  if (action != NULL)
  {
    action->vtbl->cleanup(action);
    SDL_free(action);
  }
}

void action_on_cleanup(SDL_UNUSED void* self)
{
#if DEBUG
  INFO("action cleanup\n");
#endif
}

void action_on_start(SDL_UNUSED void* self, SDL_UNUSED ecs_entity_t target)
{
#if DEBUG
  INFO("action start\n");
#endif
}

void action_on_end(SDL_UNUSED void* self, SDL_UNUSED ecs_entity_t target)
{
}

BOOL action_on_update(SDL_UNUSED void* self, SDL_UNUSED ecs_entity_t target)
{
  return TRUE;
}

//==============================================================//
//  composite action section                                    //
//==============================================================//
CompositeAction* composite_action_init(CompositeAction* self)
{
  self->children = ptr_array_new(NULL);
  return self;
}

void composite_action_cleanup(void* _self)
{
  CompositeAction* self = _self;
  int              count;
  Action**         children;

  count    = self->children->cnt;
  children = ptr_array_storage(Action, self->children);
  for (int i = 0; i < count; ++i)
    action_delete(children[i]);
  ptr_array_delete(self->children);
  self->children = NULL;
}

void composite_action_add(CompositeAction* self, Action* action)
{
  ASSERT(action != NULL);
  ptr_array_add(self->children, action);
}
#include <stdarg.h>
void composite_action_addn(CompositeAction* self, ...)
{
  va_list args;
  Action* action;
  va_start(args, self);
  while ((action = va_arg(args, Action*)) != NULL)
  {
    ptr_array_add(self->children, action);
  }
  va_end(args);
}
//==============================================================//
//  parallel action section                                     //
//==============================================================//
typedef struct ParallelAction
{
  CompositeAction parent_instance;
} ParallelAction;

static ParallelAction* parallel_action_init(ParallelAction* self)
{
  composite_action_init(COMPOSITE_ACTION(self));
  return self;
}

static void parallel_action_start(void* _self, ecs_entity_t target)
{
  Action** children;
  int      count;
  children = ptr_array_storage(Action, COMPOSITE_ACTION(_self)->children);
  count    = COMPOSITE_ACTION(_self)->children->cnt;
  for (int i = 0; i < count; ++i)
  {
    ACTION_START(children[i], target);
  }
}

static void parallel_action_end(void* _self, ecs_entity_t target)
{
  Action** children;
  int      count;
  children = ptr_array_storage(Action, COMPOSITE_ACTION(_self)->children);
  count    = COMPOSITE_ACTION(_self)->children->cnt;
  for (int i = 0; i < count; ++i)
  {
    ACTION_END(children[i], target);
  }
}

static BOOL parallel_action_update(void* _self, ecs_entity_t target)
{
  Action** children;
  int      count;
  BOOL     finished = TRUE;

  children = ptr_array_storage(Action, COMPOSITE_ACTION(_self)->children);
  count    = COMPOSITE_ACTION(_self)->children->cnt;
  for (int i = 0; i < count; ++i)
  {
    if (!ACTION_UPDATE(children[i], target))
      finished = FALSE;
  }
  return finished;
}

static ActionVtbl _parallel_action_vtbl = { .start   = parallel_action_start,
                                            .end     = parallel_action_end,
                                            .update  = parallel_action_update,
                                            .cleanup = composite_action_cleanup };

CompositeAction* parallel_action_new(void)
{
  return (CompositeAction*)parallel_action_init(
      action_alloc(ParallelAction, &_parallel_action_vtbl));
}

//==============================================================//
//  sequence action section                                     //
//==============================================================//

typedef struct SequenceAction
{
  CompositeAction parent_instance;
  int             currentChildIndex;
} SequenceAction;

#define SEQUENCE_ACTION(ptr) ((SequenceAction*)ptr)

static SequenceAction* sequence_action_init(SequenceAction* self)
{
  composite_action_init(COMPOSITE_ACTION(self));
  self->currentChildIndex = -1;
  return self;
}

void static sequence_action_start(void* _self, ecs_entity_t target)
{
  SequenceAction*  self  = _self;
  CompositeAction* super = COMPOSITE_ACTION(_self);
  if (super->children->cnt == 0)
    return;

  self->currentChildIndex = 0;
  ACTION_START(ptr_array_at(super->children, 0), target);
}
void static sequence_action_end(void* self, SDL_UNUSED ecs_entity_t target)
{
  SEQUENCE_ACTION(self)->currentChildIndex = -1;
}

INLINE Action* sequence_action_next_child(SequenceAction* self)
{
  return self->currentChildIndex < COMPOSITE_ACTION(self)->children->cnt - 1
             ? ptr_array_at(COMPOSITE_ACTION(self)->children, ++self->currentChildIndex)
             : NULL;
}

static BOOL sequence_action_update(void* _self, ecs_entity_t target)
{
  SequenceAction* self = _self;
  Action *        current_child, *next_child;
  PtrArray*       children;

  children = COMPOSITE_ACTION(self)->children;
  if (children->cnt == 0)
    return TRUE;
  current_child = ptr_array_at(children, self->currentChildIndex);
  if (ACTION_UPDATE(current_child, target))
  {
    ACTION_END(current_child, target);
    if ((next_child = sequence_action_next_child(self)) != NULL)
      ACTION_START(next_child, target);
    else
      return TRUE;
  }
  return FALSE;
}

static ActionVtbl _sequence_action_vtbl = { .start   = sequence_action_start,
                                            .end     = sequence_action_end,
                                            .update  = sequence_action_update,
                                            .cleanup = composite_action_cleanup };

CompositeAction* sequence_action_new(void)
{
  return (CompositeAction*)sequence_action_init(
      action_alloc(SequenceAction, &_sequence_action_vtbl));
}
