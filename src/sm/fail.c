#include "fail.h"
#include "config.h"
#include "sm_config.h"
#include "events/input.h"

static void prv_init_sm(void);
static struct State fail;
static struct StateMachine sm = {
  .default_state = &fail,
  .init = prv_init_sm,
  .id = SM_FAILURE
};

static void prv_kill() {
  // Disable in specific order: MPPT -> Solar -> Battery
  io_set_state(&mppt_enable, IO_LOW);

  // TODO: replace with actual timer delay - used to disable the MPPTs before they're disconnected
  // from the battery
  __delay_cycles(400);

  relay_set_state(&relay_solar, RELAY_OPEN);

  relay_set_state(&relay_battery, RELAY_OPEN);

  // Reset motor controllers - resets relays and transducer power
  mc_init(&mc_config);
}

static void prv_init_sm() {
  // Kill HV power
  // Note that all on-failure actions should have taken place already, making this state simple.
  // POWER_OFF is specified as the exit event so
  state_init(&fail, prv_kill);
}

struct StateMachine *fail_get_sm(void) {
  return &sm;
}

EventID fail_get_exit_event(void) {
  return POWER_OFF;
}
