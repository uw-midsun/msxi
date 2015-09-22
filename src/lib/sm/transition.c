/*
  transition.c - Titus Chow
  
  This abstracts transition rules from the state machine.
    It's mostly for compiler correctness, since pointers can technically be passed as integers.

  Transition rules are held in an object pool to avoid having to deal with memory allocation.
  They act as linked lists to allow a variable number of transition rules per state.

  The pool size should be adjusted to match the total number of rules in the system.

*/
#include "transition.h"
#include <assert.h>

#define NO_ACTION (struct Action) { NULL, 0, DATA }
#define TRANSITION_POOL_SIZE 20

typedef enum {
  DATA,
  POINTER
} ActionType;

struct Action {
  union {
    DataFunc fn_data;
    PointerFunc fn_pointer;
  };
  union {
    uint16_t data;
    void *pointer;
  };
  ActionType type;
};

struct TransitionRule {
  Event event;
  Guard guard;
  struct Action action;
  struct TransitionRule *next_rule;
};

static struct TransitionRule rule_pool[TRANSITION_POOL_SIZE] = { 0 };

// init_transitions() initializes the transition rule pool.
void init_transitions() {
  struct TransitionRule *temp_rule;
  for (temp_rule = rule_pool; temp_rule < rule_pool + TRANSITION_POOL_SIZE; temp_rule++) {
    temp_rule->event = NULL_EVENT;
    temp_rule->guard = NO_GUARD;
    temp_rule->action = NO_ACTION;
    temp_rule->next_rule = NULL;
  }
}

// process_transitions(transitions, sm, e) processes the transitions in the given list of transitions,
//   checking if the events match and the guard is either true or does not exist.
// Note that actions can either have integer or pointer data.
//   This is mostly for correctness, since pointers can technically be passed as ints.
bool process_transitions(struct TransitionRule *transitions, struct StateMachine *sm, Event e) {
  struct TransitionRule *rule = transitions;
  while (rule != NULL) {
    if (rule->event == e &&
      (rule->guard == NO_GUARD || rule->guard())) {
      if (rule->action.type == POINTER) {
        rule->action.fn_pointer(sm, rule->action.pointer);
      } else {
        rule->action.fn_data(sm, rule->action.data);
      }
      return true;
    }
    rule = rule->next_rule;
  }
  return false;
}

static struct TransitionRule *get_free_rule() {
  struct TransitionRule *temp_rule;
  for (temp_rule = rule_pool; temp_rule < rule_pool + TRANSITION_POOL_SIZE; temp_rule++) {
    if (temp_rule->event == NULL_EVENT) {
      return temp_rule;
    }
  }
  // Should never reach here - Increase pool if it does.
  assert(0 && "Transition Rule pool is too small!");
  return NULL;
}

static struct TransitionRule *make_transition_rule(Event e, Guard guard, struct Action action) {
  struct TransitionRule *rule = get_free_rule();
  rule->event = e;
  rule->guard = guard;
  rule->action = action;
  return rule;
}

struct TransitionRule *make_pointer_rule(Event e, Guard guard, PointerFunc fn, void *pointer) {
  return make_transition_rule(e, guard, (struct Action) { .fn_pointer = fn, .pointer = pointer, POINTER });
}

struct TransitionRule *make_data_rule(Event e, Guard guard, DataFunc fn, uint16_t data) {
  return make_transition_rule(e, guard, (struct Action) { .fn_data = fn, .data = data, DATA });
}

struct TransitionRule *add_rule(struct TransitionRule *transitions, struct TransitionRule *next_rule) {
  next_rule->next_rule = transitions;
  return next_rule;
}
