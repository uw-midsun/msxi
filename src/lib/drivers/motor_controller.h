#pragma once
#include <stdint.h>
#include <stdbool.h>

// This is for motor controller precharge and discharge.

struct MotorController;

extern const struct MotorController *LEFT_MOTORCONTROLLER, *RIGHT_MOTORCONTROLLER;

void init_motor_controllers();

bool begin_precharge(const struct MotorController *controller);

bool end_precharge(const struct MotorController *controller);

bool begin_discharge(const struct MotorController *controller);

bool end_discharge(const struct MotorController *controller);

uint16_t get_precharge_voltage(const struct MotorController *controller);

uint16_t get_discharge_voltage(const struct MotorController *controller);
