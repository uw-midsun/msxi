/*
  transition.h - Titus Chow
  
  A structure for transition rules.

*/
#ifndef TRANSITION_H_
#define TRANSITION_H_
#include "event_queue.h"
#include <stdbool.h>

#define NO_GUARD NULL
#define ELSE NULL

struct StateMachine;
struct TransitionRule;

typedef void(*DataFunc)(struct StateMachine *, uint16_t);
typedef void(*PointerFunc)(struct StateMachine *, void *);
typedef bool(*Guard)();

// init_transitions() initalizes the transition framework.
void init_transitions();

// process_transitions(transitions, sm, e) processes all the transition rules in the list, executing the first rule that matches.
void process_transitions(struct TransitionRule *transitions, struct StateMachine *sm, Event e);

// make_pointer_rule(e, guard, fn, pointer) returns a pointer-based transition rule.
struct TransitionRule *make_pointer_rule(Event e, Guard guard, PointerFunc fn, void *pointer);

// make_data_rule(e, guard, fn, data) returns a data-based transition rule.
struct TransitionRule *make_data_rule(Event e, Guard guard, DataFunc fn, uint16_t data);

// add_rule(transitions, next_rule) adds the rule to the list of transition rules, returning the new list pointer.
// Note that rules will be processed in reverse order: that is, rules are processed in LIFO order.
struct TransitionRule *add_rule(struct TransitionRule *transitions, struct TransitionRule *next_rule);

#endif /* TRANSITION_H_ */
