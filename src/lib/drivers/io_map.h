#pragma once
#include <stdint.h>

// Provides common functions and convenient mappings for interfacing with IO pins.

// Due to how we're using the macro, we can't wrap "map" in brackets.
// Thus, it's up to the user not to call IOMAP(map) with other arguments.
#define IOMAP(map) &(struct IOMap) map

#if CHAOS
#include "chaos.h"
#else
#include "launchpad.h"
#endif

struct IOMap {
  uint8_t port;
  uint16_t pins;
};

typedef enum {
  IO_LOW,
  IO_HIGH
} IOState;

typedef enum {
  PIN_IN,
  PIN_OUT
} IODirection;

void io_set_dir(const struct IOMap *map, IODirection direction);

// Enables the pin's secondary function.
void io_set_peripheral_dir(const struct IOMap *map, IODirection direction);

void io_set_state(const struct IOMap *map, const IOState state);

void io_toggle(const struct IOMap *map);

IOState io_get_state(const struct IOMap *map);
