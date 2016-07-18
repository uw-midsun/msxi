#include "run.h"
#include "run/startup.h"
#include "run/shutdown.h"
#include "config.h"
#include "sm_config.h"
#include "events/input.h"
#include "events/horn.h"
#include "events/fail.h"
#include "events/heartbeat.h"

// Startup (Handled by state machine)
// Running:
//   Horn (CAN)
//   Power Off (Input)
// Shutdown (Handled by state machine)

static void prv_init_sm(void);

static struct State startup, running, shutdown;
static struct StateMachine sm = {
  .default_state = &startup,
  .init = prv_init_sm,
  .id = SM_RUN
};

static void prv_debug(void) {
  __no_operation();
}

static void prv_init_sm() {
  state_init_composite(&startup, startup_get_sm());
  fail_add_rule(&startup, EMERGENCY_STOP, fail_handle_killswitch);
  fail_add_rule(&startup, HEARTBEAT_BAD, fail_handle_heartbeat);
  state_add_state_transition(&running, POWER_OFF, &shutdown);
  state_add_state_transition(&startup, startup_get_exit_event(), &running);

  state_init(&running, prv_debug);
  state_add_transition(&running, transitions_make_data_rule(CAN_INTERRUPT, NO_GUARD,
                                                            horn_process_message, 0));
  fail_add_rule(&running, EMERGENCY_STOP, fail_handle_killswitch);
  fail_add_rule(&running, HEARTBEAT_BAD, fail_handle_heartbeat);
  state_add_state_transition(&running, POWER_OFF, &shutdown);

  state_init_composite(&shutdown, shutdown_get_sm());
}

struct StateMachine *run_get_sm(void) {
  return &sm;
}

EventID run_get_exit_event(void) {
  return shutdown_get_exit_event();
}
