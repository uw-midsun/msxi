#pragma once
#include "sm/state_machine.h"

// Fail SM
// Kill _everything_

struct StateMachine *fail_get_sm(void);

EventID fail_get_exit_event(void);
