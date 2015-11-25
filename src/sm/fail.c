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
  // Reset relays
  relay_set_state(&relay_battery, RELAY_OPEN);
  relay_set_state(&relay_solar, RELAY_OPEN);

  // Reset motor controllers - resets relays and transducer power
  mc_init(&mc_config);
}

static void prv_init_sm() {
  state_init(&fail, prv_kill);
}

struct StateMachine *fail_get_sm(void) {
  return &sm;
}

EventID fail_get_exit_event(void) {
  return POWER_OFF;
}
