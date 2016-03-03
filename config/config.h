#pragma once
#include "drivers/io_map.h"
#include "drivers/spi.h"
#include "drivers/can.h"
#define MAX_LIGHTS 10

extern const struct SPIConfig spi_b1;

extern const struct CANConfig can;

extern const struct IOMap can_interrupt;

extern const struct IOMap left_turn_lights[MAX_LIGHTS];

extern const struct IOMap right_turn_lights[MAX_LIGHTS];

extern const struct IOMap running_lights[MAX_LIGHTS];

extern const struct IOMap brake_lights[MAX_LIGHTS];
