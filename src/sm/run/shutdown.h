#pragma once
#include "sm/state_machine.h"
#include "sm_config.h"

// Shutdown SM
// Discharge -> Kill power
// Does not handle external fail events.

typedef enum {
  SHUTDOWN_COMPLETE = STATEMACHINE_ID(SM_SHUTDOWN)
} ShutdownEvent;

struct StateMachine *shutdown_get_sm(void);

EventID shutdown_get_exit_event(void);
