#include "io_map.h"
#include "msp430.h"
#include <stdio.h>

// These maps are here because the F427 doesn't support driverlib )':
struct GPIOMap {
  volatile uint8_t *dir;
  volatile uint8_t *in;
  volatile uint8_t *out;
  volatile uint8_t *sel;
  volatile uint8_t *ie;
  volatile uint8_t *ies;
  volatile uint8_t *ifg;
};

static const struct GPIOMap GPIO[] = {
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL },
  { &P1DIR, &P1IN, &P1OUT, &P1SEL, &P1IE, &P1IES, &P1IFG },
  { &P2DIR, &P2IN, &P2OUT, &P2SEL, &P2IE, &P2IES, &P2IFG },
  { &P3DIR, &P3IN, &P3OUT, &P3SEL, NULL, NULL, NULL},
  { &P4DIR, &P4IN, &P4OUT, &P4SEL, NULL, NULL, NULL },
  { &P5DIR, &P5IN, &P5OUT, &P5SEL, NULL, NULL, NULL },
  { &P6DIR, &P6IN, &P6OUT, &P6SEL, NULL, NULL, NULL },
  { &P7DIR, &P7IN, &P7OUT, &P7SEL, NULL, NULL, NULL },
  { &P8DIR, &P8IN, &P8OUT, &P8SEL, NULL, NULL, NULL },
#if defined(__MSP430_HAS_PORT10_R__)
  { &P9DIR, &P9IN, &P9OUT, &P9SEL, NULL, NULL, NULL },
  { &P10DIR, &P10IN, &P10OUT, &P10SEL, NULL, NULL, NULL }
#endif
};

void io_set_dir(const struct IOMap *map, IODirection direction) {
  if(direction == PIN_OUT) {
    *GPIO[map->port].dir |= map->pins;
  } else {
    *GPIO[map->port].dir &= ~map->pins;
  }
}

void io_set_peripheral_dir(const struct IOMap *map, IODirection direction) {
  io_set_dir(map, direction);
  *GPIO[map->port].sel |= map->pins;
}

void io_set_state(const struct IOMap *map, const IOState state) {
  if (state == IO_HIGH) {
    *GPIO[map->port].out |= map->pins;
  } else {
    *GPIO[map->port].out &= ~map->pins;
  }
}

void io_toggle(const struct IOMap *map) {
  *GPIO[map->port].out ^= map->pins;
}

IOState io_get_state(const struct IOMap *map) {
  bool high = ((*GPIO[map->port].in & map->pins) == map->pins);
  return (high) ? IO_HIGH : IO_LOW;
}

void io_configure_interrupt(const struct IOMap *map, bool enabled, IOInterruptEdge edge) {
  if (enabled) {
    if (edge == EDGE_FALLING) {
      *GPIO[map->port].ies |= map->pins;
    } else {
      *GPIO[map->port].ies &= ~map->pins;
    }

    *GPIO[map->port].ifg &= ~map->pins;
    *GPIO[map->port].ie |= map->pins;
  } else {
    *GPIO[map->port].ie &= ~map->pins;
  }
}

bool io_process_interrupt(const struct IOMap *map) {
  if ((*GPIO[map->port].ifg & map->pins) != 0) {
    // Clear flag
    *GPIO[map->port].ifg &= ~map->pins;
    return true;
  }

  return false;
}

void io_toggle_interrupt_edge(const struct IOMap *map) {
  // 0 = EDGE_RISING, 1 = EDGE_FALLING
  if ((*GPIO[map->port].ies & map->pins) == 0) {
    *GPIO[map->port].ies |= map->pins;
  } else {
    *GPIO[map->port].ies &= ~map->pins;
  }
}
