#include "horn.h"
#include "can_config.h"
#include <stddef.h>

static const struct CANConfig *can_cfg = NULL;
static const struct IOMap *pin = NULL;

// The CANConfig should already be initialized.
void horn_init(const struct CANConfig *can, const struct IOMap *horn) {
  can_cfg = can;
  pin = horn;

  // Horn is connected to an active-high MOSFET?
  io_set_state(pin, IO_LOW);
  io_set_dir(pin, PIN_OUT);
}

// Data rule to process and act on CAN messages.
void horn_process_message(struct StateMachine *sm, uint16_t ignored) {
  struct CANMessage msg = { 0 };
  struct CANError error = { 0 };

  // Not sure if we care about CAN errors, but this clears any that may have occurred.
  while(can_process_interrupt(can_cfg, &msg, &error)) {
    if (msg.id == THEMIS_HORN) {
      // We got a horn message
      // Horn messages carry a boolean value - off or on.
      if (msg.data == 0) {
        io_set_state(pin, IO_LOW);
      } else {
        io_set_state(pin, IO_HIGH);
      }
    }
  }
}

void horn_interrupt(void) {
  if (io_process_interrupt(&can_cfg->interrupt_pin)) {
    // We need to raise an event so we can process the message in the main loop since our SPI
    // driver is interrupt-driven.
    event_raise_isr(CAN_INTERRUPT, 0);
  }
}
