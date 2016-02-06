#include "input.h"

// Called on edges of mechanical and regen brakes.
// Also called repeatedly when regen braking is active
static void prv_handle_brake(struct InputConfig *input) {
  struct BrakeInput *brake = &input->brake;
  bool mech_active = (io_get_state(&brake->mech) == IO_LOW),
       regen_active = brake->regen_state;

  if (mech_active || regen_active) {
    // raise brake event
    uint16_t regen_pot = adc12_sample(input->adc, brake->regen);
    // Scale from [low, high] to [0, 1]
    union {
      float f;
      uint32_t i;
    } regen;
    regen.f = (float)((regen_pot - brake->calibration.low) /
                      (brake->calibration.high - brake->calibration.low));

    // [63-32]: mechanical (bool), [31-0]: regen (float)
    event_raise(BRAKE_CHANGE, ((uint64_t)mech_active << 32) | regen.i);
  } else {
    // raise no brake event
    event_raise(BRAKE_CHANGE, 0);
  }
}

// TODO: Combine regen and throttle edge checks?
static void prv_check_regen(struct InputConfig *input) {
  struct BrakeInput *brake = &input->brake;
  uint16_t regen_pot = adc12_sample(input->adc, brake->regen);
  if ((brake->regen_state && (regen_pot <= brake->calibration.low)) ||
      (!brake->regen_state && (regen_pot >= brake->calibration.low))) {
    // Started or stopped regen braking
    brake->regen_state = !brake->regen_state;
    prv_handle_brake(input);
  } else if (brake->regen_state) {
    // Continued regen braking - continue to raise brake events
    prv_handle_brake(input);
  }
}

static void prv_check_throttle(struct InputConfig *input) {
  struct ThrottleInput *throttle = &input->throttle;
  uint16_t throttle_pot = adc12_sample(input->adc, throttle->throttle);
  bool update = false;

  if ((throttle->state && (throttle_pot <= throttle->calibration.low)) ||
      (!throttle->state && (throttle_pot >= throttle->calibration.high))) {
    // Throttle edge
    throttle->state = !throttle->state;
    update = true;
  } else if (throttle->state) {
    // Continued throttle updates
    update = true;
  }

  if (update) {
    union {
      float f;
      uint32_t i;
    } drive;
    drive.f = (float)((throttle_pot - throttle->calibration.low) /
                      (throttle->calibration.high - throttle->calibration.low));

    event_raise(throttle->event, drive.i);
  }
}

static void prv_check_direction(struct DirectionInput *dir) {
  // 0000 00xy: x = backward, y = forward -> 0: neutral, 1: forward, 2: reverse
  uint8_t state = (io_get_state(&dir->backward) << 1) | io_get_state(&dir->forward);

  if (state != dir->state) {
    dir->state = state;
    event_raise(dir->event, state);
  }
}

void input_poll(struct InputConfig *input) {
  int i;
  for (i = 0; i < NUM_POLLED_INPUTS; i++) {
    struct Input *in = &input->polled[i];
    if (io_get_state(&in->input) != in->state) {
      in->state = io_get_state(&in->input);
      event_raise(in->event, in->state);
    }
  }

  prv_check_direction(&input->direction);
  prv_check_throttle(input);
  prv_check_regen(input);
}

void input_process(struct InputConfig *input) {
  int i;
  for (i = 0; i < NUM_ISR_INPUTS; i++) {
    struct Input *in = &input->isr[i];
    if (io_process_interrupt(&in->input)) {
      event_raise(in->event, io_get_state(&in->input));
    }
  }

  if (io_process_interrupt(&input->brake.mech)) {
    prv_handle_brake(input);
  }
}
