#pragma once
#include "sm_config.h"
#include "sm/state_machine.h"

// State machine for MC precharge
// Precharges all MCs synchronously
// Does not handle external fail events.

typedef enum {
  PRECHARGE_BEGIN = STATEMACHINE_ID(SM_PRECHARGE),
  PRECHARGE_TIMEOUT,
  PRECHARGE_SUCCESS,
  PRECHARGE_COMPLETE,
  PRECHARGE_FAIL,
} PrechargeEvent;

struct StateMachine *precharge_get_sm(void);

EventID precharge_get_exit_event(void);
