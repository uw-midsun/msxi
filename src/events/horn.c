#include "horn.h"

static const struct CANConfig *can_cfg = NULL;
static const struct IOMap *pin = NULL;

// The CANConfig should already be initialized.
void horn_init(const struct CANConfig *can, const struct IOMap *horn) {
  can_cfg = can;
  pin = horn;

  // Horn is connected to an active-high MOSFET?
  io_set_state(pin, IO_LOW);
  io_set_dir(pin, PIN_OUT);

  // TODO: should this be moved into the driver?
  io_configure_interrupt(&can->interrupt_pin, true, EDGE_FALLING);
}

// Data rule to process CAN messages. This should be
void horn_process_message(struct StateMachine *sm, uint16_t ignored) {
  struct CANMessage msg = { 0 };
  struct CANError error = { 0 };

  // Not sure if we care about CAN errors, but this clears any that may have occurred.
  can_process_interrupt(can_cfg, &msg, &error);

  if ((msg.id & CAN_MESSAGE_MASK) == 0x123) {
    // We got a horn message (don't care about who's sending it)
    // Horn messages carry a boolean value - off or on.
    EventID e = (msg.data == 1) ? HORN_ON : HORN_OFF;
    event_raise(e, 0);
  }
}

void horn_interrupt(void) {
  if (io_process_interrupt(&can_cfg->interrupt_pin)) {
    // We need to raise an event so we can process the message in the main loop since our SPI
    // driver is interrupt-driven.
    event_raise_isr(CAN_INTERRUPT, 0);
  }
}
