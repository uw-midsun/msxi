#pragma once
#include "drivers/io_map.h"
#include "drivers/spi.h"
#include "drivers/can.h"
#include "drivers/relay.h"
#include "events/motor_controller.h"
#include "drivers/adc12.h"
#include "sm/sm_debug.h"
#include "events/input.h"

extern const struct SPIConfig spi_a0;

extern const struct CANConfig can;

extern const struct ADC12Config adc12_a;

extern const struct Relay relay_battery, relay_solar;

extern const struct MCConfig mc_config;

extern const struct IOMap plutus_heartbeat, horn, enable_lv, mppt_enable;

extern const struct SMDebugConfig sm_debug;

extern struct InputConfig input;
