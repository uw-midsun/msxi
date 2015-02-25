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

struct MotorController LEFT_CONTROLLER = { { { 1, 2 }, STATUS_PIN, { 3, 4 } },
							{ { 5, 6 }, VOLTAGE_INDEX, { 0, 1 } },
							{ { 9, 10 }, VOLTAGE_INDEX, { 0, 2 } } };
struct MotorController RIGHT_CONTROLLER = { { { 12, 11 }, STATUS_PIN, { 10, 9 } },
							{ { 8, 7 }, VOLTAGE_INDEX, { 0, 3 } },
							{ { 4, 3 }, VOLTAGE_INDEX, { 0, 4 } } };

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