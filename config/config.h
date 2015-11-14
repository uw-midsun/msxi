#pragma once
#include "drivers/io_map.h"
#include "drivers/spi.h"
#include "drivers/can.h"
#include "drivers/relay.h"
#include "drivers/motor_controller.h"
#include "drivers/adc12.h"
#include "../src/sm/sm_debug.h"

extern const struct SPIConfig spi_a0;

extern const struct CANConfig can;

extern const struct Relay relay_battery, relay_solar;

extern const struct MotorController mc_left, mc_right;

extern const struct ADC12Config adc12_a;

extern const struct IOMap plutus_heartbeat;

extern const struct SMDebugConfig sm_debug;
