#pragma once
#include "relay.h"
#include "adc12.h"

// This is for motor controller precharge and discharge.

struct MotorController {
  struct Relay enable;
  struct Relay charge;
  struct Relay discharge;
  ADC12Index charge_index;
  ADC12Index discharge_index;
};

typedef enum {
  MC_BEGIN_PRECHARGE,
  MC_END_PRECHARGE,
  MC_BEGIN_DISCHARGE,
  MC_END_DISCHARGE
} MCState;

typedef enum {
  MC_VOLTAGE_PRECHARGE,
  MC_VOLTAGE_DISCHARGE
} MCVoltageType;

void mc_init(const struct MotorController *controller);

// Returns whether the state was set correctly
bool mc_set_state(const struct MotorController *controller, const MCState state);

uint16_t mc_sample_voltage(const struct MotorController *controller, const MCVoltageType type);
