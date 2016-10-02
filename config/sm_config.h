#pragma once

// State Machine ID mapping
// State machines are given IDs for debugging and to act as offsets for their events.
// The event ID is a bitmask of xyyyyyzzzzzzzzzz where
// - x: Protected (bool)
// - y: SM/Protected ID (5 bits)
// - z: Event ID (10 bits

#define STATEMACHINE_ID(offset) ((uint16_t)(offset & 0x1F) << 10)
#define PROTECTED_EVENT_ID(offset) ((uint16_t)(offset & 0x1F) << 10 | ((uint16_t)1 << 15))

typedef enum {
  SM_MAIN = 1,
  SM_RUN,
  SM_STARTUP,
  SM_PRECHARGE,
  SM_SHUTDOWN,
  SM_DISCHARGE,
  SM_CHARGING,
  SM_FAILURE,
} StateMachineID;

typedef enum {
  EVENT_HEARTBEAT = 1,
  EVENT_INPUT,
  EVENT_HORN,
  EVENT_POWER,
  EVENT_RELAY,
  EVENT_FAILURE,
} ProtectedEventID;
