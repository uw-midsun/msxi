/*
  motor_controller.c - Titus Chow

  Global constants for the motor controllers and related functions.
  
  The motor controller has 3 relays:
	1) Main control (enable)
	2) Precharge
	3) Discharge

  In order to measure the precharge/discharge circuits, EN_MEASURE must be active.

*/
#include "motor_controller.h"
#include "relay.h"

struct MotorController {
	struct Relay enable;
	struct Relay charge;
	struct Relay discharge;
	VoltageIndex charge_index;
	VoltageIndex discharge_index;
};

static struct IOMap ENABLE_MEASUREMENT = { 1, 2 };

static struct MotorController left_controller = { { { 1, 2 }, { 3, 4 } },
							{ { 5, 6 }, { 0 } },
							{ { 9, 10 }, { 0 } },
							MCU_CHG1, MCU_DCHG1 };
static struct MotorController right_controller = { { { 12, 11 }, { 10, 9 } },
							{ { 8, 7 }, { 0 } },
							{ { 4, 3 }, { 0 } },
							MCU_CHG2, MCU_DCHG2 };

struct MotorController *LEFT_MOTORCONTROLLER = &left_controller;
struct MotorController *RIGHT_MOTORCONTROLLER = &right_controller;

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
	set_io_dir(&ENABLE_MEASUREMENT, OUT);
	set_io_high(&ENABLE_MEASUREMENT);
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
