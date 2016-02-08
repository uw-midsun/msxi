#pragma once
#include "drivers/io_map.h"
#include "drivers/adc12.h"
#include "sm/event_queue.h"

// Controls input - event generator
// Note that this generates brake and throttle events with formed drive commands as data values.

#define NO_LED_PORT 0
#define NO_LED_PIN (struct IOMap) { NO_STATUS_PORT, 0 }
#define REGEN_GAIN_RESOLUTION 6

typedef enum {
  POLLED_INPUT_OFFSET,
  CRUISE_DIR_POS,
  CRUISE_DIR_NEG,
  POLLED_INPUT_MAX
} PolledInputEvent;
#define NUM_POLLED_INPUTS (POLLED_INPUT_MAX - POLLED_INPUT_OFFSET)

typedef enum {
  ISR_INPUT_OFFSET,
  SIG_L_TOGGLE,
  SIG_R_TOGGLE,
  HAZARD_TOGGLE,
  HORN_TOGGLE,
  IGNITION_TOGGLE,
  CRUISE_TOGGLE,
  ISR_INPUT_MAX
} InterruptInputEvent;
#define NUM_ISR_INPUTS (ISR_INPUT_MAX - ISR_INPUT_OFFSET)

typedef enum {
  BRAKE_CHANGE, // This is both interrupt (mechanical) and polled (regen)
  BRAKE_EDGE,
  THROTTLE_CHANGE,
} SpecialInputEvent;

struct Input {
  EventID event;
  struct IOMap input;
  IOState state; // Used for polled switches
};

struct PotInput {
  EventID event;
  struct {
    uint16_t high;
    uint16_t low;
  } calibration;
  ADC12Index input;
  bool state;
};

struct BrakeInput {
  EventID edge;
  struct IOMap mech;
  struct IOMap gain[REGEN_GAIN_RESOLUTION]; // Regen gain: weakest to highest
  struct PotInput regen;
  bool active;
};

struct ThrottleInput {
  struct PotInput pot;
  struct {
    struct IOMap forward;
    struct IOMap backward;
  } dir;
};

struct InputConfig {
  struct ADC12Config *adc;
  struct Input polled[NUM_POLLED_INPUTS];
  struct Input isr[NUM_ISR_INPUTS];
  struct BrakeInput brake;
  struct ThrottleInput throttle;
};

void input_init(struct InputConfig *input);

// Call at least once every 250ms to generate drive commands within the motor controller's watchdog
void input_poll(struct InputConfig *input);

void input_process(struct InputConfig *input);
