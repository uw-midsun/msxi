/*
  state_machine.h - Titus Chow

  This is the framework for an object-oriented Moore? state machine.

  Each state has a specific action that it calls on entry. It is recommended to make this your initialization function.
    States also keep track of transition rules that are matched against incoming events in reverse order.
	  (i.e. given rule1, rule2, rule3: events will be matched against rule3, then rule2, then rule1.)
    The transition rules support optional guards.
  If a transition rule is called, it calls the appropriate action.
    Actions are provided with the state machine that called them and pre-defined data (either an integer or a pointer). 

  The recommended process for creating a state machine is to encapsulate each state machine in its own source and header files.
  It should only expose its associated events and pointers to the state machine and its initialization function.
  
  To initialize a composite state,
    Set sub_sm to the internal state machine,
    Set the entry function to the state machine's initialization function,
    And add a transition rule to move to the next state on completion.
  
  Some generic actions are provided, such as changing states and raising events.

*/
#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_
#include "event_queue.h"
#include <stdbool.h>

#define NO_GUARD NULL
#define ELSE NULL

typedef void(*StateFunc)();
typedef bool(*Guard)();

struct Action {
	void(*fn)(struct StateMachine *, int);
	union {
		int data;
		void *pointer;
	};
};

struct TransitionRule {
	Event event;
	Guard guard;
	struct Action action;
};

struct State {
	StateFunc enter;
	struct StateMachine *sub_sm;
	struct TransitionRuleNode *transitions;
};

struct StateMachine {
	struct State *current_state;
};

// init_sm_framework() initializes the state machine framework.
void init_sm_framework();

// process_event(sm, e) processes the given event using the provided state machine.
// If the current state is composite, then events are processed using its sub-state machine.
// requires: sm must be initialized and populated.
void process_event(struct StateMachine *sm, Event e);

// add_transition(state, rule) adds the given rule to the given state.
// requires: init_sm_framework() has been called.
void add_transition(struct State *state, struct TransitionRule *rule);

// Generic actions

// change_state(sm, next_state) changes the current state to the given state,
//   calling its entry function.
void change_state(struct StateMachine *sm, struct State *next_state);

// raise_action_event(sm, e) raises the specified event in the global event queue.
void raise_action_event(struct StateMachine *sm, Event e);

#endif