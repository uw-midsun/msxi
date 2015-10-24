#pragma once
#include "io_map.h"

// Timer-based heartbeat check for Plutus (Battery board).
// Plutus should output high on Chaos's heartbeat pin while good.

// Since this event is independent of the state machines,
//  it must not conflict with any existing events.
#define HEARTBEAT_EVENT_OFFSET 6000

typedef enum {
  HEARTBEAT_BAD = HEARTBEAT_EVENT_OFFSET,
  HEARTBEAT_GOOD
} HeartbeatEvent;

// Initializes the heartbeat check on the specified pin.
// Every ~1s, it will check to see if the pin's state has changed,
//  raising an event in the event queue if it has.
void heartbeat_begin(const struct IOMap *heartbeat_pin);
