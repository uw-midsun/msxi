#pragma once
#include "drivers/relay.h"
#include "drivers/adc12.h"
#include "sm/event_queue.h"

// Motor Controller precharge/discharge functions
// Allows the safe precharge/discharge in serial each step
// i.e. All motor controllers begin precharge, then all are checked for power state.
//      Once all motor controllers are precharged, we open the main relays.

#define MAX_MOTOR_CONTROLLERS 2

struct MotorController {
  struct Relay main;
  struct Relay charge;
  struct Relay discharge;
  ADC12Index charge_index;
  ADC12Index discharge_index;
  uint8_t id;
};

struct MCConfig {
  const struct ADC12Config *adc;
  struct IOMap enable_measure;
  uint8_t num_mc;
  const struct MotorController *mc[MAX_MOTOR_CONTROLLERS];
};

typedef enum {
  TRANSDUCER_ENABLED,
  TRANSDUCER_DISABLED
} TransducerState;

typedef bool (*SuccessFunc)(const struct MotorController *mc, const struct ADC12Config *adc);

// Resets the motor controllers.
// The ADC should already be initialized.
void mc_init(const struct MCConfig *config);

// Enables/disables the transducers
void mc_set_transducer_state(const struct MCConfig *config, const TransducerState state);

// Processes each motor controller according to the success function.
// Raises the appropriate event with the last controller processed as data.
void mc_process(const struct MCConfig *config, SuccessFunc fn, EventID on_success, EventID on_fail);

// Success Functions
// TODO: two different types of success functions: with ADC, without ADC?

// Returns whether the precharge relays were configured properly.
bool mc_precharge_begin(const struct MotorController *mc, const struct ADC12Config *adc);

// Returns whether the motor controllers have reached a safe voltage.
bool mc_precharge_power(const struct MotorController *mc, const struct ADC12Config *adc);

// Returns whether the precharge relays were configured properly.
bool mc_precharge_end(const struct MotorController *mc, const struct ADC12Config *adc);

// Returns whether the discharge relays were configured properly.
bool mc_discharge_begin(const struct MotorController *mc, const struct ADC12Config *adc);

// Returns whether the motor controllers have reached a safe voltage.
bool mc_discharge_power(const struct MotorController *mc, const struct ADC12Config *adc);

// Returns whether the discharge relays were configured properly.
bool mc_discharge_end(const struct MotorController *mc, const struct ADC12Config *adc);
