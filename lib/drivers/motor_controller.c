/*
  motor_controller.c - Titus Chow

  Global constants for the motor controllers and related functions.
  
  The motor controller has 3 relays:
	1) Main control (enable)
	2) Precharge
	3) Discharge 
*/
#include "motor_controller.h"
#include "relay.h"

struct MotorController LEFT_CONTROLLER = { { { 1, 2 }, { 3, 4 } },
							{ { 5, 6 }, { 0 } },
							{ { 9, 10 }, { 0 } },
							MCU_CHG1, MCU_DCHG1 };
struct MotorController RIGHT_CONTROLLER = { { { 12, 11 }, { 10, 9 } },
							{ { 8, 7 }, { 0 } },
							{ { 4, 3 }, { 0 } },
							MCU_CHG2, MCU_DCHG2 };

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
