#pragma once
#include "gpio_map.h"
#include <stdint.h>
#include <stdbool.h>

// Provides common functions and convenient mappings for interfacing with IO pins.

struct IOMap {
  uint8_t port;
  uint8_t pins;
};

typedef enum {
  IO_LOW,
  IO_HIGH
} IOState;

typedef enum {
  PIN_IN,
  PIN_OUT
} IODirection;

typedef enum {
  EDGE_RISING,
  EDGE_FALLING
} IOInterruptEdge;

void io_set_dir(const struct IOMap *map, IODirection direction);

// Enables the pin's secondary function.
void io_set_peripheral_dir(const struct IOMap *map, IODirection direction);

void io_set_state(const struct IOMap *map, const IOState state);

void io_toggle(const struct IOMap *map);

IOState io_get_state(const struct IOMap *map);

void io_configure_interrupt(const struct IOMap *map, bool enabled, IOInterruptEdge edge);
