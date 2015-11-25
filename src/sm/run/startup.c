#include "startup.h"
#include "precharge.h"
#include "config.h"
#include "events/heartbeat.h"
#include "events/fail.h"
#include "events/input.h"
#include "events/power.h"
#include "events/protected_relay.h"
#include "drivers/delay.h"

// Reset peripherals
// Switch to DC-DC
// Precharge (Handled by state machine)

static void prv_init_sm(void);
static struct State init, heartbeat, power, precharge;
static struct StateMachine sm = {
  .default_state = &init,
  .init = prv_init_sm,
  .id = SM_STARTUP
};

static void prv_reset_peripherals() {
  // Enable LV power
  io_set_state(&enable_lv, IO_HIGH);

  // Reset relays and LEDs
  mc_init(&mc_config);
  sm_debug_init(&sm_debug);
  relay_init(&relay_battery);
  relay_init(&relay_solar);

  // Protected relay switch will move us to the next state
  protected_relay_set_state(&relay_solar, RELAY_CLOSED);
}

static void prv_init_heartbeat() {
  // Allow Plutus to initialize if it isn't already active
  delay_seconds(1);

  heartbeat_fire_event();
}

static void prv_init_power() {
  // Enable power to the car
  protected_relay_set_state(&relay_battery, RELAY_CLOSED);
}

static void prv_init_sm() {
  state_init(&init, prv_reset_peripherals);
  state_add_state_transition(&init, RELAY_SWITCHED, &heartbeat);
  fail_add_rule(&init, RELAY_FAIL, fail_handle_relay);

  state_init(&heartbeat, prv_init_heartbeat);
  state_add_state_transition(&heartbeat, HEARBEAT_GOOD, &power);
  fail_add_rule(&heartbeat, HEARTBEAT_BAD, fail_handle_heartbeat);

  state_init(&power, prv_init_power);
  state_add_state_transition(&power, RELAY_SWITCHED, &precharge); // Could add DC-DC check here
  fail_add_rule(&power, RELAY_FAIL, fail_handle_relay);

  state_init_composite(&precharge, precharge_get_sm());
}

struct StateMachine *startup_get_sm(void) {
  return &sm;
}

EventID startup_get_exit_event(void) {
  return precharge_get_exit_event();
}
