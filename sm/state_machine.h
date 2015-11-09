#pragma once
#include "event_queue.h"
#include "transition.h"

// This is the framework for an object-oriented Moore? state machine.

// Each state has a specific action that is called on entry. It is recommended to make this
//  your initialization function.

// States have transition rules that are matched against incoming events.
// If a transition rule is matched against it's optional guard, it calls the associated action.
//  Actions are provided with the state machine that called them and pre-defined data.

// When creating a composite state, remember to add a transition rule to exit it on completion.
//  When first entered, its state machine will be lazily populated.
//  If re-entered, it will just switch back to its initial (default) state.

// Some generic actions are provided, such as changing states and raising events.

#define NO_ENTRY_FN NULL

typedef void(*EntryFunc)();
typedef void(*InitFunc)();

struct State {
  EntryFunc enter;
  struct StateMachine *sub_sm;
  struct Transitions transitions;
};

struct StateMachine {
  struct State *current_state;
  struct State *default_state;
  bool initialized;
  InitFunc init;
};

void sm_framework_init();

// Initializes a state machine by populating it and then switching it to its default state.
void sm_init(struct StateMachine *main);

void state_init(struct State *state, EntryFunc entry_fn);

// Initializes the state as a composite state with sm as its sub-state machine.
void state_init_composite(struct State *state, struct StateMachine *sm);

void sm_process_event(struct StateMachine *sm, struct Event e);

// Adds a custom transition rule to the given state.
void state_add_transition(struct State *state, struct TransitionRule *rule);

// Adds a guarded sm_change_state rule to the given state.
void state_add_guarded_state_transition(struct State *state, EventID e, Guard g, struct State *next_state);

// Adds a generic sm_change_state rule to the given state.
void state_add_state_transition(struct State *state, EventID e, struct State *next_state);

// Adds a guarded sm_raise_event rule to the given state.
void state_add_guarded_event_rule(struct State *state, EventID e, Guard g, EventID event);

// Adds a generic sm_raise_event rule to the given state.
void state_add_event_rule(struct State *state, EventID e, EventID event);

// Generic actions

// Changes the state machine's current state to the given state, calling its entry function.
void sm_change_state(struct StateMachine *sm, void *next_state);

// Raises the specified event in the global event queue with a data value of 0.
void sm_raise_event(struct StateMachine *sm, EventID e);
