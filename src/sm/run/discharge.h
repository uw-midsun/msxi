#pragma once
#include "sm_config.h"
#include "sm/state_machine.h"

// State machine for MC precharge
// Discharges all MCs synchronously
// Does not handle external fail events.

typedef enum {
  DISCHARGE_BEGIN = STATEMACHINE_ID(SM_DISCHARGE),
  DISCHARGE_TIMEOUT,
  DISCHARGE_SUCCESS,
  DISCHARGE_COMPLETE,
  DISCHARGE_FAIL,
} DischargeEvent;

struct StateMachine *discharge_get_sm(void);

EventID discharge_get_exit_event(void);
