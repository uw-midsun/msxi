#pragma once
#include "drivers/io_map.h"
#include "sm_config.h"

// Interrupt-based heartbeat check for Plutus (Battery board).
// Plutus should output high on Chaos's heartbeat pin while good.
// This technically isn't a heartbeat, but Plutus should watchdog reset in any scenario
//  that would prevent the heartbeat pin from being brought low.

// Since this event is independent of the state machines,
//  it must not conflict with any existing events.

typedef enum {
  HEARTBEAT_BAD = PROTECTED_EVENT_ID(EVENT_HEARTBEAT),
  HEARTBEAT_GOOD
} HeartbeatEvent;

// Initializes the heartbeat pin and interrupt.
void heartbeat_init(const struct IOMap *heartbeat_pin);

// Callback to be executed after a timer delay. Forces a heartbeat event to be fired.
void heartbeat_timer_cb(uint16_t elapsed_ms, void *context);
