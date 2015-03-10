/*
  enable_battery.c - Titus Chow

  This is a state machine for handling battery status and control.
  Hook BATTERY_ENABLED to leave the state machine and BATTERY_FAIL to handle errors.

  Its control flow looks like this:
    enter() -> initialize state machine
    Initializing Plutus:
      enter() -> Power Plutus through the CANBus, move to Plutus Enabled
    Plutus Enabled:
      enter() -> Begin heartbeat check.
      On Good Heartbeat -> Move to Battery Enabled
    Battery Enabled:
      enter() -> Close battery relay, raise BATTERY_ENABLED

	On error: raise BATTERY_FAIL.

*/
#include "enable_battery.h"
#include "sm/state_machine.h"
#include "drivers/heartbeat.h"
#include "drivers/relay.h"

static struct StateMachine sm = { 0 };
static struct State init_plutus, plutus_enabled,
					battery_enabled;

// Powers Plutus through the CANBus (12V)
static void power_plutus() {
	//TODO: Add CAN code
	raise_event(PLUTUS_ENABLED);
}

// Called once Plutus has powered up
// Initalizes the heartbeat check, which raises events every ~1s.
static void init_plutus_heartbeat() {
	init_heartbeat(PLUTUS_HEARTBEAT);
}

static void init_battery() {
	// TODO: switch to guards?
	if(close_relay(BATTERY_RELAY_CONFIG)) {
		raise_event(BATTERY_ENABLED);
	} else {
		raise_event(BATTERY_FAIL);
	}
}

void init_battery_sm() {
	init_state(&init_plutus, power_plutus);
	add_state_transition(&init_plutus, PLUTUS_ENABLED, &plutus_enabled);

	init_state(&plutus_enabled, init_plutus_heartbeat);
	add_state_transition(&plutus_enabled, HEARTBEAT_GOOD, &battery_enabled);
	add_event_rule(&plutus_enabled, HEARTBEAT_BAD, BATTERY_FAIL);

	init_state(&battery_enabled, init_battery);

	// Initial state
	change_state(&sm, &init_plutus);
}

struct StateMachine *get_battery_sm() {
	return &sm;
}
