#include "fail.h"
#include "config.h"
#include "sm_config.h"
#include "events/input.h"
#include "events/power.h"
#include "drivers/timer.h"

static void prv_init_sm(void);
static struct State fail, reset;
static struct StateMachine sm = {
  .default_state = &fail,
  .init = prv_init_sm,
  .id = SM_FAILURE
};

static void prv_enable_lv(uint16_t elapsed_ms, void *context) {
  power_set_lv(&enable_lv, LV_ENABLED);

  event_raise(FAIL_RESET, 0);
}

static void prv_kill() {
  io_set_state(&input.power_led, IO_LOW);

  // Disable in specific order: MPPT -> Solar -> Battery
  io_set_state(&mppt_enable, IO_LOW);

  // TODO: replace with actual timer delay - used to disable the MPPTs before they're disconnected
  // from the battery
  __delay_cycles(40);

  relay_set_state(&relay_solar, RELAY_OPEN);

  relay_set_state(&relay_battery, RELAY_OPEN);

  // Reset motor controllers - resets relays and transducer power
  mc_init(&mc_config);
}

static void prv_reset() {
  power_set_lv(&enable_lv, LV_DISABLED);

  timer_delay(1000, prv_enable_lv, NULL);
}

static void prv_init_sm() {
  // Kill HV power
  // Note that all on-failure actions should have taken place already, making this state simple.
  // POWER_OFF is specified as the exit event so
  state_init(&fail, &sm, prv_kill);
  state_add_state_transition(&fail, POWER_TOGGLE, &reset);

  state_init(&reset, &sm, prv_reset);
}

struct StateMachine *fail_get_sm(void) {
  return &sm;
}

EventID fail_get_exit_event(void) {
  return FAIL_RESET;
}
