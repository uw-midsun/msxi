/*
  io_map.h - Titus Chow

  This provides a structure to represent port/pin mappings and related functions.

*/
#pragma once
#include <stdint.h>

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

// set_io_dir(map, direction) sets the specified pin(s)' IO direction.
void set_io_dir(const struct IOMap *map, IODirection direction);

// set_io_peripheral_dir(map) sets the specified pin(s)'s IO direction, enabling their secondary function.
void set_io_peripheral_dir(const struct IOMap *map, IODirection direction);

// set_io_high(map) sets the specified pin(s) to output high.
// requires: map's IO direction has been set to OUT.
void set_io_high(const struct IOMap *map);

// set_io_low(map) sets the specified pin(s) to output low.
// requires: map's IO direction has been set to OUT.
void set_io_low(const struct IOMap *map);

// toggle_io(map) toggles the output state of the specified pin(s).
// requires: map's IO direction has been set to OUT.
void toggle_io(const struct IOMap *map);

// get_io_state(map) returns the state of the specified pin.
// requires: map's IO direction has been set to IN.
IOState get_io_state(const struct IOMap *map);
