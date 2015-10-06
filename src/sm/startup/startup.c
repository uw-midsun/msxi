#include "startup.h"
#include "sm/state_machine.h"
#include "drivers/relay.h"
#include "precharge.h"
#include "enable_battery.h"

// This is the state machine that runs the car through the startup procedure.

// It has 5 main states:
//  1) Initalize - Sets pins to their default values and directions.
//  2) Enable Battery - Handles battery check, heartbeat, etc.
//  3) Enable Solar - Handles enabling solar array.
//  4) Precharge - Precharges and enables the left and right motor controllers.
//  5) Complete - Startup complete.

#define SOLAR_RELAY_CONFIG &(struct Relay) { SOLAR_RELAY, SOLAR_STATUS }

static struct State initialize, enable_battery,
                    enable_solar, precharge_mcs,
                    complete;
static struct StateMachine sm = { .default_state = &initialize, .init = startup_sm_init };

static void prv_init_system() {
  event_raise(INIT_COMPLETE);
}

static void prv_enable_solar_relay() {
  // TODO: switch to guards?
  if(relay_set_state(SOLAR_RELAY_CONFIG, RELAY_CLOSED)) {
    event_raise(SOLAR_ENABLED);
  } else {
    event_raise(SOLAR_FAIL);
  }
}

static void prv_complete_entry() {
  event_raise(STARTUP_COMPLETE);
}

static void prv_raise_error(struct StateMachine *sm, uint16_t error) {
  // TODO: Handle errors
}

void startup_sm_init() {
  state_init(&initialize, prv_init_system);
  state_add_state_transition(&initialize, INIT_COMPLETE, &enable_battery);

  state_init_composite(&enable_battery, battery_sm_get_info());
  state_add_state_transition(&enable_battery, BATTERY_ENABLED, &enable_solar);
  state_add_transition(&enable_battery, transitions_make_data_rule(BATTERY_FAIL, NO_GUARD, prv_raise_error, 1));

  state_init(&enable_solar, prv_enable_solar_relay);
  state_add_state_transition(&enable_solar, SOLAR_ENABLED, &precharge_mcs);
  state_add_transition(&enable_solar, transitions_make_data_rule(SOLAR_FAIL, NO_GUARD, prv_raise_error, 2));

  state_init_composite(&precharge_mcs, precharge_sm_get_info());
  state_add_state_transition(&precharge_mcs, PRECHARGE_COMPLETE, &complete);
  state_add_transition(&precharge_mcs, transitions_make_data_rule(PRECHARGE_FAIL, NO_GUARD, prv_raise_error, 3));
  
  state_init(&complete, prv_complete_entry);
}

struct StateMachine *startup_sm_get_info() {
  return &sm;
}
