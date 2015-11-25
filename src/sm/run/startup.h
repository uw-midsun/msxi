#pragma once
#include "sm/state_machine.h"
#include "sm_config.h"

// Startup SM
// Initialize system -> Set up power -> Precharge (-> Fail)
// Does not handle killswitch events

struct StateMachine *startup_get_sm(void);

EventID startup_get_exit_event(void);
