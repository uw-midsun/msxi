#include "enable_battery.h"
#include "sm/state_machine.h"
#include "drivers/heartbeat.h"
#include "drivers/relay.h"
#include "drivers/adc12.h"

#include "config.h"

// This is a state machine for handling battery status and control.
// Hook BATTERY_ENABLED to leave the state machine and BATTERY_FAIL to handle errors.

// Its control flow looks like this:
//  enter() -> initialize state machine
//  Initializing Plutus:
//   enter() -> Power Plutus through the CANBus, move to Plutus Enabled
//  Plutus Enabled:
//   enter() -> Begin heartbeat check.
//    On Good Heartbeat -> Move to Enabling Battery
//  Enabling Battery:
//   enter() -> Close battery relay, check DC-DC level
//    On Good DC-DC Level -> Move to Battery Enabled
//  Battery Enabled:
//   enter() -> Close battery relay, raise BATTERY_ENABLED
// On error: raise BATTERY_FAIL.

static struct State init_plutus, plutus_enabled,
                    dcdc_check, battery_enabled;
static struct StateMachine sm = { .default_state = &init_plutus, .init = battery_sm_init };

// Powers Plutus through the CANBus (12V)
static void prv_power_canbus() {
  //TODO: Add CAN code
  event_raise(PLUTUS_ENABLED);
}

// Called once Plutus has powered up
// Initalizes the heartbeat check, which raises events every ~1s.
static void prv_init_heartbeat() {
  heartbeat_begin(&plutus_heartbeat);
}

static void prv_init_dcdc_check() {
  event_raise(BATTERY_TIMEOUT);
}

static void prv_init_battery() {
  // TODO: switch to guards?
  if(relay_set_state(&relay_battery, RELAY_CLOSED)) {
    event_raise(BATTERY_ENABLED);
  } else {
    event_raise(BATTERY_FAIL);
  }
}

static void wait(struct StateMachine *sm, uint16_t ms) {
  // TODO: write delay code
  event_raise(BATTERY_TIMEOUT);
}

// Checks if PWR_STATUS is within bounds: ~11V to ~13V
static bool prv_is_dcdc_good() {
  int power_status = adc12_sample(&adc12_a, PWR_STATUS);
  return power_status >= DCDC_LOWER_THRESHOLD && power_status <= DCDC_UPPER_THRESHOLD;
}

void battery_sm_init() {
  state_init(&init_plutus, prv_power_canbus);
  state_add_state_transition(&init_plutus, PLUTUS_ENABLED, &plutus_enabled);

  state_init(&plutus_enabled, prv_init_heartbeat);
  state_add_state_transition(&plutus_enabled, HEARTBEAT_GOOD, &battery_enabled);
  state_add_event_rule(&plutus_enabled, HEARTBEAT_BAD, BATTERY_FAIL);

  state_init(&dcdc_check, prv_init_dcdc_check);
  state_add_transition(&dcdc_check, transitions_make_data_rule(BATTERY_TIMEOUT, ELSE, wait, 1));
  state_add_guarded_state_transition(&dcdc_check, BATTERY_TIMEOUT,
                                     prv_is_dcdc_good, &battery_enabled);

  state_init(&battery_enabled, prv_init_battery);
}

struct StateMachine *battery_sm_get_info() {
  return &sm;
}
