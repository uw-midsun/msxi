#pragma once
#include "sm/state_machine.h"

// Run SM - handles normal operations
// Startup -> Running -> Off (-> Fail)

// Possible main failure conditions:
// EMERGENCY_STOP, HEARTBEAT_BAD
// See sub-SM's for other possible failure conditions.

struct StateMachine *run_get_sm(void);

EventID run_get_exit_event(void);
