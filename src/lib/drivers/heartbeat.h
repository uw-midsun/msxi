/*
  heartbeat.h - Titus Chow
  
  Timer-based heartbeat for Plutus (Battery board).
  Plutus should output high on Chaos's heartbeat pin while intialized.

  Every ~1s, it will check to see if the pin's state has changed,
    raising an event in the event queue if it has.

*/
#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_
#include "io_map.h"

// Since this event is independent of the state machines, it must not conflict with any existing events.
#define HEARTBEAT_EVENT_OFFSET 6000

typedef enum {
	HEARTBEAT_BAD = HEARTBEAT_EVENT_OFFSET,
	HEARTBEAT_GOOD
} HeartbeatEvent;

// init_heartbeat(heartbeat_pin) initalizes the heartbeat check on the specified pin.
void init_heartbeat(const struct IOMap *heartbeat_pin);

#endif /* HEARTBEAT_H_ */
