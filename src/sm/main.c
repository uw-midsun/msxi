#include "main.h"
#include "sm_config.h"
#include "events/input.h"
#include "events/fail.h"
#include "run.h"
#include "charge.h"
#include "fail.h"

static void prv_init_sm(void);
static struct State off, running, charging, failure;
static struct StateMachine sm = {
  .default_state = &off,
  .init = prv_init_sm,
  .id = SM_MAIN
};

static void prv_init_sm() {
  // Move to either RUNNING or CHARGING when the POWER_ON event is fired,
  //  depending on the switch that triggered the event.
  state_init(&off, NO_ENTRY_FN);
  state_add_guarded_state_transition(&off, POWER_ON, input_is_run, &running);
  state_add_guarded_state_transition(&off, POWER_ON, input_is_charge, &charging);

  // Handled by the RUN sub-SM.
  // In the case of a general failure condition, moves to FAIL.
  state_init_composite(&running, run_get_sm());
  state_add_state_transition(&running, FAILURE_OCCURRED, &failure);
  state_add_state_transition(&running, run_get_exit_event(), &off);

  // Handled by the CHARGE sub-SM.
  // In the case of a general failure condition, moves to FAIL.
  state_init_composite(&charging, charge_get_sm());
  state_add_state_transition(&charging, FAILURE_OCCURRED, &failure);
  state_add_state_transition(&charging, charge_get_exit_event(), &off);

  // Handled by the FAIL sub-SM.
  state_init_composite(&failure, fail_get_sm());
  state_add_state_transition(&failure, fail_get_exit_event(), &off);
}

struct StateMachine *main_get_sm(void) {
  return &sm;
}
