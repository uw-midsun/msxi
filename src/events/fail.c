#include "fail.h"
#include "can_config.h"
#include "drivers/can.h"
#include "sm/event_queue.h"

static const struct CANConfig *can_cfg;

typedef enum {
  FAIL_RELAY = 1,
  FAIL_KILLSWITCH,
  FAIL_HEARTBEAT_BAD,
  FAIL_MC_BAD,
  FAIL_DCDC_BAD
} FailReason;

void fail_init(const struct CANConfig *can) {
  can_cfg = can;
}

void fail_add_rule(struct State *state, EventID e, EventDataFunc fn) {
  state_add_transition(state, transitions_make_event_data_rule(e, NO_GUARD, fn));
}

static void prv_handle_fail(uint32_t id, uint64_t code) {
  struct CANMessage msg = {
    .id = CHAOS_FAIL,
    .data_u32 = { // Little endian - id | code
      (uint32_t)code,
      id
    }
  };

  can_transmit(can_cfg, &msg);

  // The event data is more for completeness than anything else.
  event_raise(FAILURE_OCCURRED, ((uint64_t)id << 32 | code));
}

void fail_handle_relay(struct StateMachine *sm, uint64_t code) {
  prv_handle_fail(FAIL_RELAY, code);
}

void fail_handle_killswitch(struct StateMachine *sm, uint64_t code) {
  prv_handle_fail(FAIL_KILLSWITCH, code);
}

void fail_handle_heartbeat(struct StateMachine *sm, uint64_t code) {
  prv_handle_fail(FAIL_HEARTBEAT_BAD, code);
}

void fail_handle_mc(struct StateMachine *sm, uint64_t code) {
  prv_handle_fail(FAIL_MC_BAD, code);
}

void fail_handle_dcdc(struct StateMachine *sm, uint64_t code) {
  prv_handle_fail(FAIL_DCDC_BAD, code);
}
