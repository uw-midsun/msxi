#include "shutdown.h"
#include "discharge.h"
#include "drivers/relay.h"
#include "events/power.h"
#include "config.h"

static void prv_init_sm(void);
static struct State discharge, kill;
static struct StateMachine sm = {
  .default_state = &discharge,
  .init = prv_init_sm,
  .id = SM_SHUTDOWN
};

static void prv_kill() {
  // Does it really make sense to fail in shutdown?
  relay_set_state(&relay_battery, RELAY_OPEN);
  relay_set_state(&relay_solar, RELAY_OPEN);

  // Kill LV power
  power_set_lv(&enable_lv, LV_DISABLED);
  event_raise(SHUTDOWN_COMPLETE, 0);
}

static void prv_init_sm() {
  state_init_composite(&discharge, discharge_get_sm());
  state_add_state_transition(&discharge, discharge_get_exit_event(), &kill);

  state_init(&kill, prv_kill);
}

struct StateMachine *shutdown_get_sm(void) {
  return &sm;
}

EventID shutdown_get_exit_event(void) {
  return SHUTDOWN_COMPLETE;
}
