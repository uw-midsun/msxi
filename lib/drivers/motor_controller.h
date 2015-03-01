/*
  motor_controller.h - Titus Chow

  This provides a structure for holding a motor controller's relevant data and
    useful functions for precharge and discharge.

*/

#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_
#include "relay.h"
#include "mcu_voltage.h"

struct MotorController {
	struct Relay enable;
	struct Relay charge;
	struct Relay discharge;
	VoltageIndex charge_index;
	VoltageIndex discharge_index;
};

extern struct MotorController LEFT_CONTROLLER, RIGHT_CONTROLLER;

// begin_precharge(controller) begins the precharge process.
bool begin_precharge(const struct MotorController *controller);

// end_precharge(controller) ends the precharge process,
//   enabling the motor controller.
bool end_precharge(const struct MotorController *controller);

// begin_discharge(controller) begins the discharge process,
//   disabling the motor controller.
bool begin_discharge(const struct MotorController *controller);

// end_discharge(controller) ends the discharge process.
bool end_discharge(const struct MotorController *controller);

// get_precharge_voltage(controller) returns the precharge circuit's voltage.
uint16_t get_precharge_voltage(const struct MotorController *controller);

// get_discharge_voltage(controller) returns the discharge circuit's voltage.
uint16_t get_discharge_voltage(const struct MotorController *controller);

#endif
