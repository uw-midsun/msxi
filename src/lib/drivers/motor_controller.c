#include "motor_controller.h"
#include "relay.h"
#include "mcu_voltage.h"

// Each motor controller has 3 associated relays:
//  1) Main control (enable)
//  2) Precharge
//  3) Discharge

// TODO: move to config file?

struct MotorController {
  struct Relay enable;
  struct Relay charge;
  struct Relay discharge;
  VoltageIndex charge_index;
  VoltageIndex discharge_index;
};

static const struct MotorController left_controller = {
  { LEFT_MC_RELAY, LEFT_MC_STATUS },
  { CHG_LEFT_RELAY, NO_STATUS_PIN },
  { DCHG_LEFT_RELAY, NO_STATUS_PIN },
  MCU_CHG1, MCU_DCHG1 };
static const struct MotorController right_controller = {
  { RIGHT_MC_RELAY, RIGHT_MC_STATUS },
  { CHG_RIGHT_RELAY, NO_STATUS_PIN },
  { DCHG_RIGHT_RELAY, NO_STATUS_PIN },
  MCU_CHG2, MCU_DCHG2 };

const struct MotorController *LEFT_MOTORCONTROLLER = &left_controller;
const struct MotorController *RIGHT_MOTORCONTROLLER = &right_controller;

static void init_mc(const struct MotorController *controller) {
  relay_init(&controller->charge);
  relay_init(&controller->discharge);
  relay_init(&controller->enable);
}

void init_motor_controllers() {
  init_mc(LEFT_MOTORCONTROLLER);
  init_mc(RIGHT_MOTORCONTROLLER);

  // In order to measure the precharge/discharge circuits, ENABLE_MEASUREMENT must be active.
  io_set_dir(IOMAP(ENABLE_MEASUREMENT), PIN_OUT);
  io_set_state(IOMAP(ENABLE_MEASUREMENT), IO_HIGH);
}

bool begin_precharge(const struct MotorController *controller) {
  return relay_set_state(&controller->charge, RELAY_CLOSED);
}

bool end_precharge(const struct MotorController *controller) {
  return (relay_set_state(&controller->enable, RELAY_CLOSED) &&
          relay_set_state(&controller->charge, RELAY_OPEN));
}

bool begin_discharge(const struct MotorController *controller) {
  return (relay_set_state(&controller->enable, RELAY_OPEN) &&
          relay_set_state(&controller->discharge, RELAY_CLOSED));
}

bool end_discharge(const struct MotorController *controller) {
  return relay_set_state(&controller->discharge, RELAY_OPEN);
}

uint16_t get_precharge_voltage(const struct MotorController *controller) {
  return mcu_voltage_sample(controller->charge_index);
}

uint16_t get_discharge_voltage(const struct MotorController *controller) {
  return mcu_voltage_sample(controller->discharge_index);
}
