#include "controls.h"
#include "drivers/can.h"
#include "input.h"
#include "mc_state.h"
#include "speed.h"
#include "can/config.h"
#include "config.h"

// Off => [Drive (=> CC) => Brake => Drive] (=> Off)

static void prv_init_sm(void);
static void prv_init_run_sm(void);
static struct State off, run, drive, cruise, brake;
static struct StateMachine sm = {
  .default_state = &off,
  .init = prv_init_sm
}, run_sm = {
  .default_state = &drive,
  .init = prv_init_run_sm
};

static void prv_drive_command(struct StateMachine *sm, uint64_t data) {
  // Drive commands should already be formed - just pass through to the motor controllers
  struct CANMessage msg = {
    .id = THEMIS_DRIVE,
    .data = data
  };
  can_transmit(&can, &msg);
}

// Cruise related functions ->
static float target_velocity = 0.0f;

static void prv_cruise_command(struct StateMachine *sm, uint64_t data) {
  struct CANMessage msg = {
    .id = THEMIS_DRIVE,
    .data_f = { target_velocity, 1.0f }
  };

  can_transmit(&can, &msg);
}

static bool prv_override_cruise(uint64_t data) {
  union {
    uint32_t data;
    float current;
  } throttle;

  // Truncate velocity data
  throttle.data = data;

  // TODO: Check if this works - goal: if requested throttle > current speed, pass through
  return (throttle.current > mc_state_value(&mc_state, MC_AVERAGE, MC_CURRENT));
}

static void prv_cruise_increment(struct StateMachine *sm, void *ignored) {
  target_velocity += speed_to(CRUISE_CONTROL_INTERVAL);
}

static void prv_cruise_decrement(struct StateMachine *sm, void *ignored) {
  target_velocity -= speed_to(CRUISE_CONTROL_INTERVAL);
  if (target_velocity < 0) {
    // Prevent velocity from going below 0
    target_velocity = 0;
  }
}

uint8_t controls_cruise_target() {
  return speed_convert(&target_velocity);
}

// <- End cruise

// Run SM
static void prv_init_run_sm() {
  // Drive: Process throttle events
  // If brake is activated, switch to brake
  // If cruise control is activated, switch to cruise control
  state_init(&drive, NO_ENTRY_FN);
  state_add_transition(&drive, transitions_make_event_data_rule(THROTTLE_CHANGE, NO_GUARD,
                                                                prv_drive_command));
  state_add_guarded_state_transition(&drive, BRAKE_EDGE, input_rising_edge, &brake);
  state_add_guarded_state_transition(&drive, CRUISE_TOGGLE, input_rising_edge, &cruise);

  // Brake: Process brake events
  // If brake is no longer activated, switch to drive
  state_init(&brake, NO_ENTRY_FN);
  state_add_transition(&brake, transitions_make_event_data_rule(BRAKE_CHANGE, NO_GUARD,
                                                                prv_drive_command));
  state_add_guarded_state_transition(&brake, BRAKE_EDGE, input_falling_edge, &drive);

  // Cruise: Override throttle events with constant velocity packets
  // If the throttle event is faster than the current velocity, allow it to override cruise control
  // If brake is activated, switch to brake
  // If cruise control is disabled, switch to drive
  state_init(&cruise, NO_ENTRY_FN);
  state_add_transition(&cruise, transitions_make_event_data_rule(THROTTLE_CHANGE, prv_override_cruise,
                                                                 prv_drive_command));
  state_add_transition(&cruise, transitions_make_event_data_rule(THROTTLE_CHANGE, ELSE,
                                                                 prv_cruise_command));
  state_add_guarded_state_transition(&cruise, BRAKE_EDGE, input_rising_edge, &brake);
  state_add_guarded_state_transition(&cruise, CRUISE_TOGGLE, input_falling_edge, &drive);
}

// Main SM
static void prv_init_sm() {
  // Off
  state_init(&off, NO_ENTRY_FN);
  state_add_guarded_state_transition(&off, IGNITION_TOGGLE, input_rising_edge, &run);

  // On
  // Allow target cruise velocity to be modified whenever the car is on
  state_init_composite(&run, &run_sm);
  state_add_transition(&run, transitions_make_pointer_rule(CRUISE_DIR_POS, input_rising_edge,
                                                           prv_cruise_increment, NULL));
  state_add_transition(&run, transitions_make_pointer_rule(CRUISE_DIR_NEG, input_rising_edge,
                                                           prv_cruise_decrement, NULL));
  state_add_guarded_state_transition(&run, IGNITION_TOGGLE, input_falling_edge, &off);
}

struct StateMachine *controls_sm(void) {
  return &sm;
}
