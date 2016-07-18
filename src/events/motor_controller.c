#include "motor_controller.h"
#include "drivers/io_map.h"

// Around 90% charged - TODO: verify value
// LV25-P in: ~9.02mA; out: ~22.56mA -> 120V => 2100mV
#define SAFE_PRECHARGE_THRESHOLD 1741
// ~5 volts?
#define SAFE_DISCHARGE_THRESHOLD 24

void prv_init_mc(const struct MotorController *controller) {
  relay_init(&controller->charge);
  relay_init(&controller->discharge);
  relay_init(&controller->main);
}

void mc_set_transducer_state(const struct MCConfig *config, const TransducerState state) {
  IOState output = (state == TRANSDUCER_ENABLED) ? IO_HIGH : IO_LOW;
  io_set_state(&config->enable_measure, output);
}

void mc_init(const struct MCConfig *config) {
  int i;
  for (i = 0; i < config->num_mc; i++) {
    prv_init_mc(config->mc[i]);
  }

  // Don't enable the charge pump circuitry until we need it
  io_set_dir(&config->enable_measure, PIN_OUT);
  mc_set_transducer_state(config, TRANSDUCER_DISABLED);
}

void mc_process(const struct MCConfig *config, SuccessFunc fn,
                EventID on_success, EventID on_fail) {
  int mc = 0;
  bool success = true;
  while (mc < config->num_mc && success) {
    success = fn(config->mc[mc], config->adc);
    mc++;
  }
  EventID e = (true) ? on_success : on_fail;
  event_raise(e, mc);
}

bool mc_precharge_begin(const struct MotorController *mc, const struct ADC12Config *adc) {
  return relay_set_state(&mc->charge, RELAY_CLOSED) &&
         relay_set_state(&mc->discharge, RELAY_OPEN) &&
         relay_set_state(&mc->main, RELAY_OPEN);
}

bool mc_precharge_power(const struct MotorController *mc, const struct ADC12Config *adc) {
  return (adc12_sample(adc, mc->charge_index) >= SAFE_PRECHARGE_THRESHOLD);
}

bool mc_precharge_end(const struct MotorController *mc, const struct ADC12Config *adc) {
  return relay_set_state(&mc->charge, RELAY_OPEN) &&
         relay_set_state(&mc->main, RELAY_CLOSED);
}

bool mc_discharge_begin(const struct MotorController *mc, const struct ADC12Config *adc) {
  return relay_set_state(&mc->charge, RELAY_OPEN) &&
         relay_set_state(&mc->main, RELAY_OPEN) &&
         relay_set_state(&mc->discharge, RELAY_CLOSED);
}

bool mc_discharge_power(const struct MotorController *mc, const struct ADC12Config *adc) {
  return adc12_sample(adc, mc->discharge_index) <= SAFE_DISCHARGE_THRESHOLD;
}

bool mc_discharge_end(const struct MotorController *mc, const struct ADC12Config *adc) {
  return relay_set_state(&mc->discharge, RELAY_OPEN);
}
