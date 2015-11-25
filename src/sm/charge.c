#include "charge.h"
#include "config.h"
#include "drivers/delay.h"
#include "events/heartbeat.h"
#include "events/protected_relay.h"
#include "events/fail.h"
#include "events/input.h"

static void prv_init_sm(void);
static struct State enable, heartbeat, charging;
static struct StateMachine sm = {
  .default_state = &enable,
  .init = prv_init_sm,
  .id = SM_CHARGING
};

static void prv_enable() {
  // Ensure that LV power is enabled - powers the rest of the boards
  // Note that LV power is generally enabled.
  io_set_state(&enable_lv, IO_HIGH);
  protected_relay_set_state(&relay_solar, RELAY_CLOSED);
}

static void prv_enable_heartbeat() {
  // Allow Plutus to initialize if it isn't already active
  delay_seconds(1);
  // Heartbeat check should already be active by now
  heartbeat_fire_event();
}

static void prv_charge() {
  // We don't really care if we're powered off of DC-DC or not. Do we?
  relay_set_state(&relay_battery, RELAY_CLOSED);
}

static void prv_init_sm() {
  state_init(&enable, prv_enable);
  fail_add_rule(&enable, RELAY_FAIL, fail_handle_relay); // Could disable LV power
  state_add_state_transition(&enable, RELAY_SWITCHED, &heartbeat);

  state_init(&heartbeat, prv_enable_heartbeat);
  fail_add_rule(&heartbeat, HEARTBEAT_BAD, fail_handle_heartbeat);
  state_add_state_transition(&heartbeat, HEARBEAT_GOOD, &charging);

  state_init(&charging, prv_charge);
  fail_add_rule(&heartbeat, HEARTBEAT_BAD, fail_handle_heartbeat);
  state_add_event_rule(&charging, POWER_OFF, CHARGING_COMPLETE);
}


struct StateMachine *charge_get_sm(void) {
  return &sm;
}

EventID charge_get_exit_event(void) {
  return CHARGING_COMPLETE;
}
