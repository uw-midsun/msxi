#include "startup.h"
#include "precharge.h"
#include "config.h"
#include "drivers/timer.h"
#include "events/heartbeat.h"
#include "events/fail.h"
#include "events/input.h"
#include "events/power.h"
#include "events/protected_relay.h"

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
  power_set_lv(&enable_lv, LV_ENABLED);

  // Reset relays and LEDs
  mc_init(&mc_config);
  sm_debug_init(&sm_debug);
  relay_init(&relay_battery);
  relay_init(&relay_solar);
  io_set_state(&input.power_led, IO_HIGH);

  // Protected relay switch will move us to the next state
  protected_relay_set_state(&relay_battery, RELAY_CLOSED);
}

static void prv_init_heartbeat() {
  // Allow Plutus to initialize if it isn't already active
  timer_delay(1000, heartbeat_timer_cb, (void *)true);
}

static void prv_init_solar() {
  // Attach solar array to the main circuit
  protected_relay_set_state(&relay_solar, RELAY_CLOSED);

  // TODO: replace with actual timer delay - used to enable the MPPTs only once they're attached
  // to the battery
  __delay_cycles(400);

  // Enable MPPTs - active-high
  io_set_state(&mppt_enable, IO_HIGH);
}

static void prv_init_sm() {
  state_init(&init, &sm, prv_reset_peripherals);
  state_add_state_transition(&init, RELAY_SWITCHED, &heartbeat);
  fail_add_rule(&init, RELAY_FAIL, fail_handle_relay);

  state_init(&heartbeat, &sm, prv_init_heartbeat);
  state_add_state_transition(&heartbeat, HEARTBEAT_GOOD, &power);
  fail_add_rule(&heartbeat, HEARTBEAT_BAD, fail_handle_heartbeat);

  state_init(&power, &sm, prv_init_solar);
  state_add_state_transition(&power, RELAY_SWITCHED, &precharge); // Could add DC-DC check here
  fail_add_rule(&power, RELAY_FAIL, fail_handle_relay);

  state_init_composite(&precharge, &sm, precharge_get_sm());
}

struct StateMachine *startup_get_sm(void) {
  return &sm;
}

EventID startup_get_exit_event(void) {
  return precharge_get_exit_event();
}
