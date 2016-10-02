#include "misc/fraction.h"
#include "input.h"
#include "drivers/led.h"

typedef void (*PotHandler)(struct InputConfig *);

#include "config.h"
#include <stdio.h>

// Return [63-32]: Velocity, [31-0]: Current
static uint64_t prv_build_payload(const float velocity, const float current) {
  union {
    uint64_t data;
    struct {
      float velocity;
      float current;
    };
  } payload;

  payload.velocity = velocity;
  payload.current = current;

  return payload.data;
}

// Scale from [high, low] to [0, scale]
static Fraction prv_scale_pot(struct InputConfig *input, struct PotInput *pot,
                              const Fraction scale) {
  const uint16_t pot_value = adc12_sample(input->adc, pot->input);
  const Fraction percent = fraction_from(pot->calibration.high - pot_value,
                                         pot->calibration.high - pot->calibration.low);
  const Fraction scaled_percent = fraction_mult(percent, scale),
                 min = fraction_from_i(0);

  if (scaled_percent < min) {
    return min;
  } else if (scaled_percent >= scale) {
    return scale;
  } else {
    return scaled_percent;
  }
}

static Fraction prv_scale_gain(struct BrakeInput *brake) {
  uint8_t numerator;
  for (numerator = 0; numerator < REGEN_GAIN_RESOLUTION; numerator++) {
    // Active-low: Only one can be active at a time
    if (io_get_state(&brake->gain[numerator]) == IO_LOW) {
      break;
    }
  }

  if (numerator >= REGEN_GAIN_RESOLUTION) {
    numerator = 0;
  }

//  return fraction_from(numerator + 1, REGEN_GAIN_RESOLUTION);
  return fraction_from_i(1);
}

// Handles both regen and mechanical braking
// Regen braking: 0m/s velocity, [0, 1]% current
static void prv_handle_brake(struct InputConfig *input) {
  struct BrakeInput *brake = &input->brake;
  bool mech_active = io_get_state(&brake->mech),
       regen_active = brake->regen.state,
       brake_active = mech_active || regen_active;

  if (brake_active != brake->active) {
    brake->active = brake_active;
    // Raise edge event - 0 = falling, 1 = rising
    event_raise(brake->edge, brake->active);
  }

  // Limit max regen current with regen gain
  const Fraction regen_gain = prv_scale_gain(brake),
                 regen_percent = prv_scale_pot(input, &brake->regen, regen_gain);

  const uint16_t pot_value = adc12_sample(input->adc, brake->regen.input);

  static char display_buffer[20];
  snprintf(display_buffer, 20, "B: %.4f / %d", fraction_to(regen_percent), pot_value);
  lcd_println(display.lcd, LINE_2, display_buffer);

  event_raise(brake->regen.event, prv_build_payload(0, fraction_to(regen_percent)));
}

static void prv_handle_throttle(struct InputConfig *input) {
  static const float dir_velocity[] = {
    0.0f,
    100.0f,
    -100.0f
  };
  struct ThrottleInput *throttle = &input->throttle;

  // 0: Neutral, 1: Forward, 2: Backward
  uint8_t dir_index = ((io_get_state(&throttle->dir.backward) << 1) |
                       io_get_state(&throttle->dir.forward));

  float current = 0;
  // Freewheel unless a direction is specified
  if (dir_index != 0) {
    const Fraction scale = fraction_from_i(1);
    current = fraction_to(prv_scale_pot(input, &throttle->pot, scale));
  }

  const uint16_t pot_value = adc12_sample(input->adc, throttle->pot.input);

//  static char display_buffer[20];
//  snprintf(display_buffer, 20, "T: %.4f / %d", current, pot_value);
//  lcd_println(display.lcd, LINE_1, display_buffer);

  event_raise(throttle->pot.event, prv_build_payload(dir_velocity[dir_index], current));
}

static void prv_check_pot(struct InputConfig *input, struct PotInput *pot, PotHandler handle_fn) {
  uint16_t pot_value = adc12_sample(input->adc, pot->input);
  pot->state = (pot_value <= pot->calibration.high - 50);
//  pot->state = 0;

  handle_fn(input);
}

void input_init(struct InputConfig *input) {
  adc12_init(input->adc);

  uint8_t i;
  for (i = 0; i < NUM_POLLED_INPUTS; i++) {
    io_set_dir(&input->polled[i].input, PIN_IN);
  }

  for (i = 0; i < NUM_ISR_INPUTS; i++) {
    io_set_dir(&input->isr[i].input, PIN_IN);
    io_configure_interrupt(&input->isr[i].input, true, EDGE_RISING); // Active-high switches
  }

  io_set_dir(&input->throttle.dir.forward, PIN_IN);
  io_set_dir(&input->throttle.dir.backward, PIN_IN);

  io_set_dir(&input->brake.mech, PIN_IN);
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
      // Active-high switches
      event_raise(in->event, in->state);
    }
  }

  prv_check_pot(input, &input->throttle.pot, prv_handle_throttle);
  prv_check_pot(input, &input->brake.regen, prv_handle_brake);
}

void input_process(struct InputConfig *input) {
  uint16_t i;
  for (i = 0; i < NUM_ISR_INPUTS; i++) {
    struct Input *in = &input->isr[i];
    if (io_process_interrupt(&in->input)) {
      IOState state = io_get_state(&in->input);
      if (in->state != state) {
        in->state = state;
        io_toggle_interrupt_edge(&in->input);
        // Active-high switches
        event_raise_isr(in->event, state);
      }
    }
  }
}

bool input_rising_edge(uint64_t data) {
  return data == 1;
}

bool input_falling_edge(uint64_t data) {
  return data == 0;
}
