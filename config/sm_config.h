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
  SM_OFF = 1,
  SM_RUNNING,
  SM_POWER_INIT,
  SM_PRECHARGE,
  SM_SHUTDOWN,
  SM_CHARGING,
  SM_FAILURE
} StateMachineID;

typedef enum {
  EVENT_HEARTBEAT = 1,
  EVENT_INPUT,
  EVENT_HORN
} ProtectedEventID;
