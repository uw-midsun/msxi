/*
  io_map.c - Titus Chow
  
  This provides common functions for interfacing with IO pins.
  It also provides a useful abstraction from the driver lib.

*/
#include "io_map.h"
#include "gpio.h"

void set_io_dir(const struct IOMap *map, IODirection direction) {
	if(direction == PIN_OUT) {
		GPIO_setAsOutputPin(map->port, map->pins);
	} else {
		GPIO_setAsInputPin(map->port, map->pins);
	}
}

void set_io_peripheral_dir(const struct IOMap *map, IODirection direction) {
	if(direction == PIN_OUT) {
		GPIO_setAsPeripheralModuleFunctionOutputPin(map->port, map->pins);
	} else {
		GPIO_setAsPeripheralModuleFunctionInputPin(map->port, map->pins);
	}
}

void set_io_high(const struct IOMap *map) {
	GPIO_setOutputHighOnPin(map->port, map->pins);
}

void set_io_low(const struct IOMap *map) {
	GPIO_setOutputLowOnPin(map->port, map->pins);
}

void toggle_io(const struct IOMap *map) {
	GPIO_toggleOutputOnPin(map->port, map->pins);
}

IOState get_io_state(const struct IOMap *map) {
	return (IOState)GPIO_getInputPinValue(map->port, map->pins);
}
