#pragma once
#include "event_queue.h"
#include <stdbool.h>

// This abstracts transition rules from the state machine framework.

#define NO_GUARD NULL
#define ELSE NULL

struct StateMachine;
struct TransitionRule;

typedef void(*DataFunc)(struct StateMachine *, uint16_t);
typedef void(*PointerFunc)(struct StateMachine *, void *);
typedef bool(*Guard)();

// init_transitions() initalizes the transition framework.
void transitions_init();

// Processes all the transition rules in the list, executing the first rule that matches.
// Returns true if a rule was matched, false if not.
bool transitions_process(struct TransitionRule *transitions, struct StateMachine *sm, Event e);

// Returns a pointer-based transition rule.
struct TransitionRule *transitions_make_pointer_rule(Event e, Guard guard, PointerFunc fn, void *pointer);

// Returns an integer-based transition rule.
struct TransitionRule *transitions_make_data_rule(Event e, Guard guard, DataFunc fn, uint16_t data);

// Adds the rule to the list of transition rules, returning the new list pointer.
// Note that rules will be processed in reverse order: that is, rules are processed in LIFO order.
struct TransitionRule *transitions_add_rule(struct TransitionRule *transitions, struct TransitionRule *next_rule);
