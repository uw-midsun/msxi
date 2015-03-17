/*
  startup.c - Titus Chow

  This is the state machine that runs the car through the startup procedure.

  It has 5 main states:
    1) Initalize - Sets pins to their default values and directions.
    2) Enable Battery - Handles battery check, heartbeat, etc.
    3) Enable Solar - Handles enabling solar array.
    4) Precharge - Precharges and enables the left and right motor controllers.
    5) Complete - Startup complete.

*/

#include "startup.h"
#include "sm/state_machine.h"
#include "drivers/relay.h"
#include "precharge.h"
#include "enable_battery.h"

#define SOLAR_RELAY_CONFIG &(struct Relay) { SOLAR_RELAY, SOLAR_STATUS }

static struct StateMachine main = { 0 };
static struct State initialize, enable_battery,
					enable_solar, precharge_mcs,
					complete;

static void init_system() {
	raise_event(INIT_COMPLETE);
}

static void enable_solar_relay() {
	// TODO: switch to guards?
	if(close_relay(SOLAR_RELAY_CONFIG)) {
		raise_event(SOLAR_ENABLED);
	} else {
		raise_event(SOLAR_FAIL);
	}
}

static void complete_entry() {
	raise_event(STARTUP_COMPLETE);
}

static void raise_error(struct StateMachine *sm, uint16_t error) {
	// TODO: Handle errors
}

void init_startup_sm() {
	init_state(&initialize, init_system);
	add_state_transition(&initialize, INIT_COMPLETE, &precharge_mcs);

	init_composite_state(&enable_battery, init_battery_sm, get_battery_sm());
	add_state_transition(&enable_battery, BATTERY_ENABLED, &enable_solar);
	add_transition(&enable_battery, make_data_rule(BATTERY_FAIL, NO_GUARD, raise_error, 1));

	init_state(&enable_solar, enable_solar_relay);
	add_state_transition(&enable_solar, SOLAR_ENABLED, &precharge_mcs);
	add_transition(&enable_solar, make_data_rule(SOLAR_FAIL, NO_GUARD, raise_error, 2));

	init_composite_state(&precharge_mcs, init_precharge_sm, get_precharge_sm());
	add_state_transition(&precharge_mcs, PRECHARGE_COMPLETE, &complete);
	add_transition(&precharge_mcs, make_data_rule(PRECHARGE_FAIL, NO_GUARD, raise_error, 3));
	
	init_state(&complete, complete_entry);

	change_state(&main, &initialize);
}

struct StateMachine *get_startup_sm() {
	return &main;
}
