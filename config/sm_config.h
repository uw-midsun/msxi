#pragma once

// State Machine ID mapping
// State machines are given IDs for debugging and to act as offsets for their events.
// SM event IDs should follow the pattern xyyzz where
// - x is reserved for non-SM events
// - y is the SM ID
// - z is the event offset

#define STATEMACHINE_ID(offset) ((offset) * 100)
#define PROTECTED_EVENT_ID(offset) ((offset) * 10000)

typedef enum {
  SM_MAIN = 1,
  SM_RUN,
  SM_STARTUP,
  SM_PRECHARGE,
  SM_SHUTDOWN,
  SM_DISCHARGE,
  SM_CHARGING,
  SM_FAILURE,
  FORCE_SM_ID_SIZE = 65535 // Forces it to be a uint16_t
} StateMachineID;

typedef enum {
  EVENT_HEARTBEAT = 1,
  EVENT_INPUT,
  EVENT_HORN,
  EVENT_POWER,
  EVENT_RELAY,
  EVENT_FAILURE,
  FORCE_EVENT_ID_SIZE = 65535 // Forces it to be a uint16_t
} ProtectedEventID;
