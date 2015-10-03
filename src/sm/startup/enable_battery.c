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
      On Good Heartbeat -> Move to Enabling Battery
    Enabling Battery:
      enter() -> Close battery relay, check DC-DC level
      On Good DC-DC Level -> Move to Battery Enabled
    Battery Enabled:
      enter() -> Close battery relay, raise BATTERY_ENABLED

  On error: raise BATTERY_FAIL.

*/
#include "enable_battery.h"
#include "sm/state_machine.h"
#include "drivers/heartbeat.h"
#include "drivers/relay.h"
#include "drivers/mcu_voltage.h"

#define BATTERY_RELAY_CONFIG &(struct Relay) { BATTERY_RELAY, BATTERY_STATUS }

static struct State init_plutus, plutus_enabled,
          dcdc_check, battery_enabled;
static struct StateMachine sm = { .default_state = &init_plutus, .init = init_battery_sm };

// Powers Plutus through the CANBus (12V)
static void power_plutus() {
  //TODO: Add CAN code
  raise_event(PLUTUS_ENABLED);
}

// Called once Plutus has powered up
// Initalizes the heartbeat check, which raises events every ~1s.
static void init_plutus_heartbeat() {
  init_heartbeat(IOMAP(PLUTUS_HEARTBEAT));
}

static void init_dcdc_check() {
  raise_event(BATTERY_TIMEOUT);
}

static void init_battery() {
  // TODO: switch to guards?
  if(close_relay(BATTERY_RELAY_CONFIG)) {
    raise_event(BATTERY_ENABLED);
  } else {
    raise_event(BATTERY_FAIL);
  }
}

static void wait(struct StateMachine *sm, uint16_t ms) {
  // TODO: write delay code
  raise_event(BATTERY_TIMEOUT);
}

// Checks if PWR_STATUS is within bounds: ~11V to ~13V
static bool is_dcdc_good() {
  int power_status = get_voltage(PWR_STATUS);
  return power_status >= DCDC_LOWER_THRESHOLD && power_status <= DCDC_UPPER_THRESHOLD;
}

void init_battery_sm() {
  init_state(&init_plutus, power_plutus);
  add_state_transition(&init_plutus, PLUTUS_ENABLED, &plutus_enabled);

  init_state(&plutus_enabled, init_plutus_heartbeat);
  add_state_transition(&plutus_enabled, HEARTBEAT_GOOD, &battery_enabled);
  add_event_rule(&plutus_enabled, HEARTBEAT_BAD, BATTERY_FAIL);

  init_state(&dcdc_check, init_dcdc_check);
  add_transition(&dcdc_check, make_data_rule(BATTERY_TIMEOUT, ELSE, wait, 1));
  add_guarded_state_transition(&dcdc_check, BATTERY_TIMEOUT, is_dcdc_good, &battery_enabled);

  init_state(&battery_enabled, init_battery);
}

struct StateMachine *get_battery_sm() {
  return &sm;
}
