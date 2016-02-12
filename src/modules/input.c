#include <misc/ratio.h>
#include "input.h"
#include "drivers/led.h"

typedef void (*PotHandler)(struct InputConfig *);

// Scale from [low, high] to [0, scale]
// Return [63-32]: Velocity, [31-0]: Current
static uint64_t prv_scale_pot(struct InputConfig *input, struct PotInput *pot,
                              const struct Ratio *scale, float velocity) {
  const uint16_t pot_value = adc12_sample(input->adc, pot->input);
  union {
    uint64_t data;
    struct {
      float velocity;
      float current;
    };
  } drive;

  drive.velocity = velocity;
  const struct Ratio percent = {pot_value - pot->calibration.low,
                                pot->calibration.high - pot->calibration.low};
  const struct Ratio current = ratio32_mult(&percent, scale);
  drive.current = ratio_to(&current);
  return drive.data;
}

// TODO: use interrupts instead?
static struct Ratio prv_scale_gain(struct BrakeInput *brake) {
  uint8_t numerator;
  for (numerator = 0; numerator < REGEN_GAIN_RESOLUTION; numerator++) {
    // Active-low: Only one can be active at a time
    if (io_get_state(&brake->gain[numerator]) == IO_LOW) {
      break;
    }
  }

  return (struct Ratio){numerator + 1, REGEN_GAIN_RESOLUTION + 1};
}

// Handles both regen and mechanical braking
// Regen braking: 0m/s velocity, [0, 1]% current
static void prv_handle_brake(struct InputConfig *input) {
  struct BrakeInput *brake = &input->brake;
  bool mech_active = (io_get_state(&brake->mech) == IO_LOW),
       regen_active = brake->regen.state,
       brake_active = mech_active || regen_active;

  if (brake_active != brake->active) {
    brake->active = brake_active;
    // Raise edge event - 0 = falling, 1 = rising
    event_raise(brake->edge, brake->active);
  }

  // Limit max regen current with regen gain
  struct Ratio regen_gain = prv_scale_gain(brake);
  event_raise(brake->regen.event, prv_scale_pot(input, &brake->regen, &regen_gain, 0.0f));
}

static void prv_handle_throttle(struct InputConfig *input) {
  static const float dir_velocity[3] = { 0.0f, 100.0f, -100.0f };
  static const struct Ratio one = {1, 1};
  struct ThrottleInput *throttle = &input->throttle;

  // TODO: do we need a throttle edge event?

  // 0: Neutral, 1: Forward, 2: Backward
  uint8_t dir_index = ((io_get_state(&throttle->dir.backward) << 1) |
                       io_get_state(&throttle->dir.forward));
  uint64_t command = prv_scale_pot(input, &throttle->pot, &one, dir_velocity[dir_index]);
  event_raise(throttle->pot.event, command);
}

static void prv_check_pot(struct InputConfig *input, struct PotInput *pot, PotHandler handle_fn) {
  uint16_t pot_value = adc12_sample(input->adc, pot->input);

  if ((pot->state && (pot_value <= pot->calibration.low)) ||
      (!pot->state && (pot_value >= pot->calibration.high))) {
    // Pot edge
    pot->state = !pot->state;
  }

  handle_fn(input);
}

void input_init(struct InputConfig *input) {
  uint8_t i;
  for (i = 0; i < NUM_POLLED_INPUTS; i++) {
    io_set_dir(&input->polled[i].input, PIN_IN);
  }

  for (i = 0; i < NUM_ISR_INPUTS; i++) {
    io_set_dir(&input->isr[i].input, PIN_IN);
    io_configure_interrupt(&input->isr[i].input, true, EDGE_FALLING); // Active-low switches
  }

  io_set_dir(&input->throttle.dir.forward, PIN_IN);
  io_set_dir(&input->throttle.dir.backward, PIN_IN);

  io_set_dir(&input->brake.mech, PIN_IN);
  io_configure_interrupt(&input->isr[i].input, true, EDGE_FALLING); // Active-low switch
  for (i = 0; i < REGEN_GAIN_RESOLUTION; i++) {
    io_set_dir(&input->brake.gain[i], PIN_IN);
  }
}

void input_poll(struct InputConfig *input) {
  uint8_t i;
  for (i = 0; i < NUM_POLLED_INPUTS; i++) {
    struct Input *in = &input->polled[i];
    if (io_get_state(&in->input) != in->state) {
      in->state = io_get_state(&in->input);
      // Active-low switches -> convert to conventional logic (i.e. active = 1)
      event_raise(in->event, (in->state == IO_LOW));
    }
  }

  prv_check_pot(input, &input->throttle.pot, prv_handle_throttle);
  prv_check_pot(input, &input->brake.regen, prv_handle_brake);
}

void input_process(struct InputConfig *input) {
  int i;
  for (i = 0; i < NUM_ISR_INPUTS; i++) {
    struct Input *in = &input->isr[i];
    if (io_process_interrupt(&in->input)) {
      IOState state = io_get_state(&in->input);
      io_toggle_interrupt_edge(&in->input);
      // Active-low switches -> convert to conventional logic (i.e. active = 1)
      event_raise(in->event, (state == IO_LOW));
    }
  }

  if (io_process_interrupt(&input->brake.mech)) {
    io_toggle_interrupt_edge(&input->brake.mech);
    prv_handle_brake(input);
  }
}

bool input_rising_edge(uint64_t data) {
  return data == 1;
}

bool input_falling_edge(uint64_t data) {
  return data == 0;
}
