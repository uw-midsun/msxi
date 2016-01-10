#pragma once
#include "sm/state_machine.h"

// Fail SM
// Kill HV power and discharge the motor controllers.
// Assumes any desired on-failure actions have already taken place.

// Note that the main purpose of the fail state is to only allow a reset
//  once the driver has pressed the power button to turn the car off.

struct StateMachine *fail_get_sm(void);

EventID fail_get_exit_event(void);
