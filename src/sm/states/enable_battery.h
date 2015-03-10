/*
  enable_battery.h - Titus Chow

  This is a state machine that handles battery status and control.
  To use this as a composite state, create a state with
    enter -> init_battery_sm
	sub_sm -> get_battery_sm()
	and add a transition rule that handles BATTERY_ENABLED to escape the state machine.

*/
#ifndef ENABLE_BATTERY_H_
#define ENABLE_BATTERY_H_

#define BATTERY_RELAY_CONFIG &(struct Relay) { *BATTERY_RELAY, *BATTERY_STATUS }

// Arbitrary event id offset - not necessary, but makes debugging easier.
#define BATTERY_EVENT_OFFSET 1100

typedef enum {
	PLUTUS_ENABLED = BATTERY_EVENT_OFFSET,
	BATTERY_ENABLED,
	BATTERY_FAIL
} BatteryEvent;

// init_precharge_sm() sets up the precharge state machine.
void init_battery_sm();

// get_precharge_sm() returns a pointer to the state machine for use in event handling.
struct StateMachine *get_battery_sm();

#endif
