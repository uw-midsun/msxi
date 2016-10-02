#include "discharge.h"
#include "config.h"
#include "events/motor_controller.h"
#include "events/fail.h"
#include "drivers/timer.h"

// Motor controller discharge
// Init -> Discharge (Loop) -> End (-> Fail)

static void prv_init_sm(void);
static struct State init, discharge, end;
static struct StateMachine sm = {
  .default_state = &init,
  .init = prv_init_sm,
  .id = SM_DISCHARGE
};

static void prv_begin_discharge() {
  // Enable power to transducers
  mc_set_transducer_state(&mc_config, TRANSDUCER_ENABLED);

  // Allow the charge pump and transducers to power up?
  mc_process(&mc_config, mc_discharge_begin, DISCHARGE_BEGIN, DISCHARGE_FAIL);
}

static void prv_raise_event(uint16_t elapsed_ms, void *context) {
  event_raise(DISCHARGE_SUCCESS, mc_config.num_mc);
}

static void prv_check_discharge() {
  timer_delay(45000, prv_raise_event, NULL);
//  mc_process(&mc_config, mc_discharge_power, DISCHARGE_SUCCESS, DISCHARGE_TIMEOUT);
}

static void prv_end_discharge() {
  // Disable +/-12V to transducers no matter what happens
  mc_set_transducer_state(&mc_config, TRANSDUCER_DISABLED);

  mc_process(&mc_config, mc_discharge_end, DISCHARGE_COMPLETE, DISCHARGE_FAIL);
}

static void prv_handle_fail(struct StateMachine *sm, uint64_t mc) {
  // Disable +/-12V to transducers
  mc_set_transducer_state(&mc_config, TRANSDUCER_DISABLED);

  // Raise generic fail event
  fail_handle_mc(sm, mc);
}

static void prv_init_sm() {
  // Begin discharge of motor controllers in parallel
  // Fail on DISCHARGE_FAIL (from failed relay)
  state_init(&init, &sm, prv_begin_discharge);
  state_add_state_transition(&init, DISCHARGE_BEGIN, &discharge);
  state_add_transition(&init, transitions_make_event_data_rule(DISCHARGE_FAIL, NO_GUARD,
                                                               prv_handle_fail));

  // Loop until discharge is complete
  state_init(&discharge, &sm, prv_check_discharge);
  state_add_state_transition(&discharge, DISCHARGE_TIMEOUT, &discharge); // Loop
  state_add_state_transition(&discharge, DISCHARGE_SUCCESS, &end);

  // End discharge
  // Fail on DISCHARGE_FAIL (from failed relay)
  state_init(&end, &sm, prv_end_discharge);
  state_add_transition(&end, transitions_make_event_data_rule(DISCHARGE_FAIL, NO_GUARD,
                                                              prv_handle_fail));
}

struct StateMachine *discharge_get_sm(void) {
  return &sm;
}

EventID discharge_get_exit_event(void) {
  return DISCHARGE_COMPLETE;
}
