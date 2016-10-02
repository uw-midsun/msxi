#pragma once
#include "drivers/io_map.h"
#include "drivers/adc12.h"
#include "sm/event_queue.h"
#include "sm_config.h"

// Controls input - event generator
// Note that this generates brake and throttle events with formed drive commands as data values.

#define NO_LED_PORT 0
#define NO_LED_PIN (struct IOMap) { NO_STATUS_PORT, 0 }
#define REGEN_GAIN_RESOLUTION 6

typedef enum {
  POLLED_INPUT_OFFSET = SM_EVENT_OFFSET(POLLED_INPUT),
  CRUISE_DIR_POS,
  CRUISE_DIR_NEG,
  POLLED_INPUT_MAX
} PolledInputEvent;
#define NUM_POLLED_INPUTS (POLLED_INPUT_MAX - POLLED_INPUT_OFFSET - 1)

typedef enum {
  ISR_INPUT_OFFSET = SM_EVENT_OFFSET(ISR_INPUT),
  SIG_L_TOGGLE,
  SIG_R_TOGGLE,
  HAZARD_TOGGLE,
  HORN_TOGGLE,
  IGNITION_TOGGLE,
  CRUISE_TOGGLE,
  HEADLIGHTS_TOGGLE,
  ISR_INPUT_MAX
} InterruptInputEvent;
#define NUM_ISR_INPUTS (ISR_INPUT_MAX - ISR_INPUT_OFFSET - 1)

typedef enum {
  BRAKE_CHANGE = SM_EVENT_OFFSET(SPECIAL_INPUT), // Regen (polled) and mechanical (polled)
  BRAKE_EDGE,
  THROTTLE_CHANGE,
  THROTTLE_EDGE
} SpecialInputEvent;

struct Input {
  EventID event;
  struct IOMap input;
  IOState state; // Used for polled switches
};

struct PotInput {
  EventID event;
  struct {
    uint16_t high; // In mV (0 to 3300)
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
  const struct ADC12Config *adc;
  struct Input polled[NUM_POLLED_INPUTS];
  struct Input isr[NUM_ISR_INPUTS];
  struct BrakeInput brake;
  struct ThrottleInput throttle;
};

void input_init(struct InputConfig *input);

// Call at least once every 250ms within an interrupt
void input_poll(struct InputConfig *input);

// Call within the appropriate IO interrupts
void input_process(struct InputConfig *input);

bool input_rising_edge(uint64_t data);

bool input_falling_edge(uint64_t data);
