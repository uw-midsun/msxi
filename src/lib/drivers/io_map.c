#include "io_map.h"
#include "gpio.h"

void io_set_dir(const struct IOMap *map, IODirection direction) {
  if(direction == PIN_OUT) {
    GPIO_setAsOutputPin(map->port, map->pins);
  } else {
    GPIO_setAsInputPin(map->port, map->pins);
  }
}

void io_set_peripheral_dir(const struct IOMap *map, IODirection direction) {
  if(direction == PIN_OUT) {
    GPIO_setAsPeripheralModuleFunctionOutputPin(map->port, map->pins);
  } else {
    GPIO_setAsPeripheralModuleFunctionInputPin(map->port, map->pins);
  }
}

void io_set_state(const struct IOMap *map, const IOState state) {
  if (state == IO_HIGH) {
    GPIO_setOutputHighOnPin(map->port, map->pins);
  } else {
    GPIO_setOutputLowOnPin(map->port, map->pins);
  }
}

void io_toggle(const struct IOMap *map) {
  GPIO_toggleOutputOnPin(map->port, map->pins);
}

IOState io_get_state(const struct IOMap *map) {
  return (IOState)GPIO_getInputPinValue(map->port, map->pins);
}
