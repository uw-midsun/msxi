#include "input.h"
#include "drivers/led.h"

typedef void (*PotHandler)(struct InputConfig *);

// Scale from [low, high] to [0, 1.0f]
static uint32_t prv_scale_pot(struct InputConfig *input, struct PotInput *pot) {
  uint16_t pot_value = adc12_sample(input->adc, pot->input);
  union {
    float f;
    uint32_t i;
  } scaled;

  scaled.f = ((float)(pot_value - pot->calibration.low) /
              (pot->calibration.high - pot->calibration.low));
  return scaled.i;
}

// Called on edges of mechanical and regen brakes.
// Also called repeatedly when regen braking is active
static void prv_handle_brake(struct InputConfig *input) {
  struct BrakeInput *brake = &input->brake;
  bool mech_active = (io_get_state(&brake->mech) == IO_LOW),
       regen_active = brake->regen.state;

  if (mech_active || regen_active) {
    // raise brake event
    // [63-32]: mechanical (bool), [31-0]: regen (float)
    uint32_t regen = prv_scale_pot(input, &input->brake.regen);
    event_raise(brake->regen.event, ((uint64_t)mech_active << 32) | regen);
  } else {
    // raise no brake event
    event_raise(brake->regen.event, 0);
  }
}

static void prv_handle_throttle(struct InputConfig *input) {
  struct PotInput *throttle = &input->throttle;

  event_raise(throttle->event, prv_scale_pot(input, throttle));
}

static void prv_check_pot(struct InputConfig *input, struct PotInput *pot, PotHandler update_fn) {
  uint16_t pot_value = adc12_sample(input->adc, pot->input);

  if ((pot->state && (pot_value <= pot->calibration.low)) ||
      (!pot->state && (pot_value >= pot->calibration.high))) {
    // Pot edge
    pot->state = !pot->state;
    update_fn(input);
  } else if (pot->state) {
    // Continued pot updates
    update_fn(input);
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
  prv_check_pot(input, &input->throttle, prv_handle_throttle);
  prv_check_pot(input, &input->brake.regen, prv_handle_brake);
}

void input_process(struct InputConfig *input) {
  int i;
  for (i = 0; i < NUM_ISR_INPUTS; i++) {
    struct Input *in = &input->isr[i];
    if (io_process_interrupt(&in->input)) {
      IOState state = io_get_state(&in->input);
      if (&in->led.port != NO_LED_PORT) {
        // Inputs are active-low
        led_set_state(&in->led, (LEDState)state);
      }
      event_raise(in->event, state);
    }
  }

  if (io_process_interrupt(&input->brake.mech)) {
    prv_handle_brake(input);
  }
}
