#include "charge.h"
#include "config.h"
#include "events/heartbeat.h"
#include "events/protected_relay.h"
#include "events/fail.h"
#include "events/input.h"
#include "events/power.h"
#include "drivers/timer.h"

static void prv_init_sm(void);
static struct State enable, heartbeat, charging, disable;
static struct StateMachine sm = {
  .default_state = &enable,
  .init = prv_init_sm,
  .id = SM_CHARGING
};

static void prv_enable() {
  // Ensure that LV power is enabled - powers the rest of the boards
  // Note that LV power is generally enabled.
  power_set_lv(&enable_lv, LV_ENABLED);
  protected_relay_set_state(&relay_battery, RELAY_CLOSED);
}

static void prv_enable_heartbeat() {
  // Allow Plutus to initialize if it isn't already active
  timer_delay(1000, heartbeat_timer_cb, NULL);
}

static void prv_charge() {
  // We don't really care if we're powered off of DC-DC or not. Do we?
  relay_set_state(&relay_solar, RELAY_CLOSED);
}

static void prv_disable() {
  // Disable in specific order: MPPT -> Solar -> Battery
  io_set_state(&mppt_enable, IO_LOW);

  // TODO: replace with actual timer delay - used to disable the MPPTs before they're disconnected
  // from the battery
  __delay_cycles(400);

  relay_set_state(&relay_solar, RELAY_OPEN);

  relay_set_state(&relay_battery, RELAY_OPEN);

  event_raise(CHARGING_COMPLETE, 0);
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
  state_add_state_transition(&charging, POWER_OFF, &disable);

  state_init(&disable, prv_disable);
  fail_add_rule(&heartbeat, HEARTBEAT_BAD, fail_handle_heartbeat);
}


struct StateMachine *charge_get_sm(void) {
  return &sm;
}

EventID charge_get_exit_event(void) {
  return CHARGING_COMPLETE;
}
