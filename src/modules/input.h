#pragma once
#include "drivers/io_map.h"
#include "drivers/adc12.h"
#include "sm/event_queue.h"

// Controls input - event generator

typedef enum {
  POLLED_INPUT_OFFSET,
  CRUISE_CHANGE,
  CRUISE_DIR_CHANGE,
  POLLED_INPUT_MAX
} PolledInputEvent;
#define NUM_POLLED_INPUTS (POLLED_INPUT_MAX - POLLED_INPUT_OFFSET)

typedef enum {
  ISR_INPUT_OFFSET,
  SIG_L_CHANGE,
  SIG_R_CHANGE,
  HAZARD_CHANGE,
  HORN_CHANGE,
  IGNITION_CHANGE,
  ISR_INPUT_MAX
} InterruptInputEvent;
#define NUM_ISR_INPUTS (ISR_INPUT_MAX - ISR_INPUT_OFFSET)

typedef enum {
  BRAKE_CHANGE, // This is both interrupt (mechanical) and polled (regen)
  THROTTLE_CHANGE,
  DIRECTION_CHANGE // 0: neutral, 1: forward, 2: reverse
} SpecialInputEvent;

struct Input {
  EventID event;
  struct IOMap input;
  struct IOMap led;
  IOState state; // Used for polled switches
};

// TODO: Combine throttle + brake
struct BrakeInput {
  EventID event;
  struct IOMap mech;
  struct {
    uint16_t high;
    uint16_t low;
  } calibration;
  ADC12Index regen;
  bool regen_state; // Last state of regen brake
};

struct ThrottleInput {
  EventID event;
  struct {
    uint16_t high;
    uint16_t low;
  } calibration;
  ADC12Index throttle;
  bool state;
};

struct DirectionInput {
  EventID event;
  struct IOMap forward;
  struct IOMap backward;
  uint8_t state;
};

struct InputConfig {
  struct ADC12Config *adc;
  struct Input polled[NUM_POLLED_INPUTS];
  struct Input isr[NUM_ISR_INPUTS];
  struct BrakeInput brake;
  struct ThrottleInput throttle;
  struct DirectionInput direction;
};

void input_init(struct InputConfig *input);

void input_poll(struct InputConfig *input);

void input_process(struct InputConfig *input);
