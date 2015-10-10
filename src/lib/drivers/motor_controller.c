#include "motor_controller.h"
#include "config.h"

// Each motor controller has 3 associated relays:
//  1) Main control (enable)
//  2) Precharge
//  3) Discharge

void mc_init(const struct MotorController *controller) {
  relay_init(&controller->charge);
  relay_init(&controller->discharge);
  relay_init(&controller->enable);
}

bool mc_set_state(const struct MotorController *controller, const MCState state) {
  switch(state) {
    case MC_BEGIN_PRECHARGE:
      return relay_set_state(&controller->charge, RELAY_CLOSED);
    case MC_END_PRECHARGE:
      return (relay_set_state(&controller->enable, RELAY_CLOSED) &&
              relay_set_state(&controller->charge, RELAY_OPEN));
    case MC_BEGIN_DISCHARGE:
      return (relay_set_state(&controller->enable, RELAY_OPEN) &&
              relay_set_state(&controller->discharge, RELAY_CLOSED));
    case MC_END_DISCHARGE:
      return relay_set_state(&controller->discharge, RELAY_OPEN);
    default:
      return false;
  }
}

uint16_t mc_sample_voltage(const struct MotorController *controller, const MCVoltageType type) {
  ADC12Index index = (type == MC_VOLTAGE_PRECHARGE) ?
                      controller->charge_index : controller->discharge_index;
  return adc12_sample(&adc12_a, index);
}
