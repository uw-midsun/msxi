#include "transition.h"
#include <assert.h>
  
// This is mostly for compiler correctness, since pointers can technically be passed as integers.

// Transition rules are held in an object pool to avoid having to deal with memory allocation.
// They act as linked lists to allow a variable number of transition rules per state.

// The pool size should be adjusted to match the total number of rules in the system.

#define NO_ACTION (struct Action) { NULL, 0, DATA }
#define TRANSITION_POOL_SIZE 100

typedef enum {
  DATA,
  POINTER,
  EVENT_DATA
} ActionType;

struct Action {
  union {
    DataFunc fn_data;
    PointerFunc fn_pointer;
    EventDataFunc fn_edata;
  };
  union {
    uint16_t data;
    void *pointer;
  };
  ActionType type;
};

struct TransitionRule {
  uint16_t event;
  Guard guard;
  struct Action action;
  struct TransitionRule *next_rule;
};

static struct TransitionRule rule_pool[TRANSITION_POOL_SIZE] = { 0 };

// Initializes the transition rule pool.
void transitions_init(void) {
  struct TransitionRule *temp_rule;
  for (temp_rule = rule_pool; temp_rule < rule_pool + TRANSITION_POOL_SIZE; temp_rule++) {
    temp_rule->event = NULL_EVENT_ID;
    temp_rule->guard = NO_GUARD;
    temp_rule->action = NO_ACTION;
    temp_rule->next_rule = NULL;
  }
}

// Note that actions can either have integer, pointer, or event data.
bool transitions_process(struct Transitions *transitions, struct StateMachine *sm,
                         struct Event *e) {
  struct TransitionRule *rule = transitions->root;
  while (rule != NULL) {
    if (rule->event == e->id &&
      (rule->guard == NO_GUARD || rule->guard(e->data))) {
      if (rule->action.type == POINTER) {
        rule->action.fn_pointer(sm, rule->action.pointer);
      } else if (rule->action.type == DATA){
        rule->action.fn_data(sm, rule->action.data);
      } else if (rule->action.type == EVENT_DATA) {
        rule->action.fn_edata(sm, e->data);
      }
      return true;
    }
    rule = rule->next_rule;
  }
  return false;
}

static struct TransitionRule *prv_get_free_rule() {
  struct TransitionRule *temp_rule;
  for (temp_rule = rule_pool; temp_rule < rule_pool + TRANSITION_POOL_SIZE; temp_rule++) {
    if (temp_rule->event == NULL_EVENT_ID) {
      return temp_rule;
    }
  }
  // Should never reach here - Increase pool if it does.
  assert(0 && "Transition Rule pool is too small!");
  return NULL;
}

static struct TransitionRule *prv_make_transition_rule(EventID event, Guard guard,
                                                       struct Action action) {
  struct TransitionRule *rule = prv_get_free_rule();
  rule->event = event;
  rule->guard = guard;
  rule->action = action;
  return rule;
}

struct TransitionRule *transitions_make_pointer_rule(EventID event, Guard guard,
                                                     PointerFunc fn, void *pointer) {
  return prv_make_transition_rule(event, guard, (struct Action) {
    .fn_pointer = fn,
    .pointer = pointer,
    .type = POINTER
  });
}

struct TransitionRule *transitions_make_data_rule(EventID event, Guard guard,
                                                  DataFunc fn, uint16_t data) {
  return prv_make_transition_rule(event, guard, (struct Action) {
    .fn_data = fn,
    .data = data,
    .type = DATA
  });
}

struct TransitionRule *transitions_make_event_data_rule(uint16_t event, Guard guard,
                                                        EventDataFunc fn) {
  return prv_make_transition_rule(event, guard, (struct Action) {
    .fn_edata = fn,
    .data = 0, // ignored
    .type = EVENT_DATA
  });
}

void transitions_add_rule(struct Transitions *transitions, struct TransitionRule *next_rule) {
  if (transitions->root == NULL) {
    // No transitions yet - initialize as root
    transitions->root = next_rule;
  } else {
    transitions->last->next_rule = next_rule;
  }

  transitions->last = next_rule;
}

// Generic Guards
bool guard_is_zero(uint64_t data) {
  return (data == 0);
}

bool guard_is_one(uint64_t data) {
  return (data == 1);
}
