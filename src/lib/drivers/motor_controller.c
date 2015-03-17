/*
  motor_controller.c - Titus Chow

  Global constants for the motor controllers and related functions.
  
  The motor controller has 3 relays:
	1) Main control (enable)
	2) Precharge
	3) Discharge

  In order to measure the precharge/discharge circuits, ENABLE_MEASUREMENT must be active.

*/
#include "motor_controller.h"
#include "relay.h"
#include "mcu_voltage.h"

struct MotorController {
	struct Relay enable;
	struct Relay charge;
	struct Relay discharge;
	VoltageIndex charge_index;
	VoltageIndex discharge_index;
};

static const struct MotorController left_controller = { { LEFT_MC_RELAY, LEFT_MC_STATUS },
							{ CHG_LEFT_RELAY, NO_STATUS_PIN },
							{ DCHG_LEFT_RELAY, NO_STATUS_PIN },
							MCU_CHG1, MCU_DCHG1 };
static const struct MotorController right_controller = { { RIGHT_MC_RELAY, RIGHT_MC_STATUS },
							{ CHG_RIGHT_RELAY, NO_STATUS_PIN },
							{ DCHG_RIGHT_RELAY, NO_STATUS_PIN },
							MCU_CHG2, MCU_DCHG2 };

const struct MotorController *LEFT_MOTORCONTROLLER = &left_controller;
const struct MotorController *RIGHT_MOTORCONTROLLER = &right_controller;

// init_mc(controller) initalizes the relays for the specified controller.
static void init_mc(const struct MotorController *controller) {
	init_relay(&controller->charge);
	init_relay(&controller->discharge);
	init_relay(&controller->enable);
}

// init_motor_controllers() initializes the relays for each motor controller and enables the measurement circuits.
void init_motor_controllers() {
	init_mc(LEFT_MOTORCONTROLLER);
	init_mc(RIGHT_MOTORCONTROLLER);
	set_io_dir(IOMAP(ENABLE_MEASUREMENT), PIN_OUT);
	set_io_high(IOMAP(ENABLE_MEASUREMENT));
}

bool begin_precharge(const struct MotorController *controller) {
	return close_relay(&controller->charge);
}

bool end_precharge(const struct MotorController *controller) {
	return (close_relay(&controller->enable) && open_relay(&controller->charge));
}

bool begin_discharge(const struct MotorController *controller) {
	return (open_relay(&controller->enable) && close_relay(&controller->discharge));
}

bool end_discharge(const struct MotorController *controller) {
	return open_relay(&controller->discharge);
}

uint16_t get_precharge_voltage(const struct MotorController *controller) {
	return get_voltage(controller->charge_index);
}

uint16_t get_discharge_voltage(const struct MotorController *controller) {
	return get_voltage(controller->discharge_index);
}
