#include "io_map.h"
#include "msp430.h"
#include <stdio.h>

// These maps are here because the F427 doesn't support driverlib )':
static volatile uint8_t *GPIO_PORT_DIR[] = { NULL, &P1DIR, &P2DIR, &P3DIR, &P4DIR, &P5DIR, &P6DIR },
                        *GPIO_PORT_IN[] = { NULL, &P1IN, &P2IN, &P3IN, &P4IN, &P5IN },
                        *GPIO_PORT_OUT[] = { NULL, &P1OUT, &P2OUT, &P3OUT, &P4OUT, &P5OUT },
                        *GPIO_PORT_SEL[] = { NULL, &P1SEL, &P2SEL, &P3SEL, &P4SEL, &P5SEL },
                        *GPIO_PORT_IE[] = { NULL, &P1IE, &P2IE },
                        *GPIO_PORT_IES[] = { NULL, &P1IES, &P2IES },
                        *GPIO_PORT_IFG[] = { NULL, &P1IFG, &P2IFG };

void io_set_dir(const struct IOMap *map, IODirection direction) {
  if(direction == PIN_OUT) {
    *GPIO_PORT_DIR[map->port] |= map->pins;
  } else {
    *GPIO_PORT_DIR[map->port] &= ~map->pins;
  }
}

void io_set_peripheral_dir(const struct IOMap *map, IODirection direction) {
  io_set_dir(map, direction);
  *GPIO_PORT_SEL[map->port] |= map->pins;
}

void io_set_state(const struct IOMap *map, const IOState state) {
  if (state == IO_HIGH) {
    *GPIO_PORT_OUT[map->port] |= map->pins;
  } else {
    *GPIO_PORT_OUT[map->port] &= ~map->pins;
  }
}

void io_toggle(const struct IOMap *map) {
  *GPIO_PORT_OUT[map->port] ^= map->pins;
}

IOState io_get_state(const struct IOMap *map) {
  bool high = ((*GPIO_PORT_IN[map->port] & map->pins) == map->pins);
  return (high) ? IO_HIGH : IO_LOW;
}

void io_configure_interrupt(const struct IOMap *map, bool enabled, IOInterruptEdge edge) {
  if (enabled) {
    if (edge == EDGE_FALLING) {
      *GPIO_PORT_IES[map->port] |= map->pins;
    } else {
      *GPIO_PORT_IES[map->port] &= ~map->pins;
    }

    *GPIO_PORT_IFG[map->port] &= ~map->pins;
    *GPIO_PORT_IE[map->port] |= map->pins;
  } else {
    *GPIO_PORT_IE[map->port] &= ~map->pins;
  }
}
