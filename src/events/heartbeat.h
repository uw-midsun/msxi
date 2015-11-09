#pragma once
#include "drivers/io_map.h"

// Interrupt-based heartbeat check for Plutus (Battery board).
// Plutus should output high on Chaos's heartbeat pin while good.

// Since this event is independent of the state machines,
//  it must not conflict with any existing events.

typedef enum {
  HEARTBEAT_CHANGE = 6000
} HeartbeatEvent;

// Initializes the heartbeat check on the specified pin.
// Every ~1s, it will check to see if the pin's state has changed,
//  raising an event in the event queue if it has.
void heartbeat_begin(const struct IOMap *heartbeat_pin);

// Heartbeat guards - use with HEARTBEAT_CHANGE to respond to heartbeat events.
bool heartbeat_good(uint64_t data);

bool heartbeat_bad(uint64_t data);
