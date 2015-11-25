#pragma once
#include "sm/state_machine.h"

// Run SM - handles normal operations
// Startup -> Running -> Off (-> Fail)

struct StateMachine *run_get_sm(void);

EventID run_get_exit_event(void);
