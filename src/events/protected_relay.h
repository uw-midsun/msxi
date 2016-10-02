#pragma once
#include "drivers/relay.h"
#include "sm_config.h"

// Event-protected relay
// Functions the same as a normal relay, but raises success and failure events.

typedef enum {
  RELAY_SWITCHED = PROTECTED_EVENT_ID(EVENT_RELAY),
  RELAY_FAIL
} RelayEvent;

bool protected_relay_set_state(const struct Relay *relay, const RelayState state);
