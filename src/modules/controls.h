#pragma once
#include "sm/state_machine.h"

// Drive controls state machine
// Handles input events and sends them to the motor controllers
// Note that motor controllers must receive a drive command at least once every 250ms.
// Thus, input events must be sent to the controls module at least once every 250ms.

#define CRUISE_CONTROL_INTERVAL 5

// Returns the target cruise speed in the specified units (See speed.h)
uint8_t controls_cruise_target(void);

bool controls_cruise_active(void);

bool controls_faulted(void);

struct StateMachine *controls_sm(void);
