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

#define HEARTBEAT_EVENT_ID 1000
#define PLUTUS_HEARTBEAT (IOMap) { 4, 7 }

typedef enum {
	HEARTBEAT_BAD = HEARTBEAT_EVENT_ID,
	HEARTBEAT_GOOD
} HeartbeatEvent;

// init_heartbeat(heartbeat_pin) initalizes the heartbeat check on the specified pin.
void init_heartbeat(const struct IOMap *heartbeat_pin);

#endif /* HEARTBEAT_H_ */
