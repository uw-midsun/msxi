#pragma once
#include "event_queue.h"
#include <stdbool.h>

// This abstracts transition rules from the state machine framework.

#define NO_GUARD NULL
#define ELSE NULL

struct StateMachine;
struct TransitionRule;

struct Transitions {
  struct TransitionRule *root;
  struct TransitionRule *last;
};

typedef void(*DataFunc)(struct StateMachine *, uint16_t);
typedef void(*PointerFunc)(struct StateMachine *, void *);
typedef bool(*Guard)(uint64_t);

// Initalizes the transition framework
void transitions_init(void);

// Processes all the transition rules in the list, executing the first rule that matches.
// Returns true if a rule was matched, false if not.
bool transitions_process(struct Transitions *transitions, struct StateMachine *sm, struct Event e);

// Returns a pointer-based transition rule.
struct TransitionRule *transitions_make_pointer_rule(uint16_t event, Guard guard,
                                                     PointerFunc fn, void *pointer);

// Returns an integer-based transition rule.
struct TransitionRule *transitions_make_data_rule(uint16_t event, Guard guard,
                                                  DataFunc fn, uint16_t data);

// Adds the rule to the list of transition rules
void transitions_add_rule(struct Transitions *transitions, struct TransitionRule *next_rule);

// Generic Guards
bool guard_is_zero(uint64_t data);

bool guard_is_one(uint64_t data);
