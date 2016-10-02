#include "mc_state.h"
#include "can/config.h"

void mc_state_update(struct MCStateConfig *state, EventID e) {
  struct CANMessage msg = { 0 };

  // Not sure if we care about CAN errors, but this clears any that may have occurred.
//  while (can_process_interrupt(state->can, &msg, &error)) {
  if (io_get_state(&state->can->interrupt_pin) == IO_LOW) {
    can_receive(state->can, &msg);
    uint8_t i;
    for (i = 0; i < MC_COUNT; i++) {
      if (msg.id == state->mc[i].packet[0].id) {
        state->mc[i].packet[0].data_u64 = msg.data;
        break;
      } else if (msg.id == state->mc[i].packet[1].id) {
        state->mc[i].packet[1].data_u64 = msg.data;
        break;
      }
    }
    _nop();

    // Super hacky - please fix
    if (msg.id == PLUTUS_FAULT) {
      event_raise(PLUTUS_CAN_FAULT, msg.data);
    }
  }
}

float mc_state_value(struct MCStateConfig *state, MotorController mc, MCStateValue val) {
  uint8_t packet = (val >> 1) & 0x01,
          data = val & 0x01;

  if (mc == MC_AVERAGE) {
    float total = 0.0f;
    uint8_t i;
    for (i = 0; i < MC_COUNT; i++) {
      if (state->mc[i].reversed) {
        total -= state->mc[i].packet[packet].data[data];
      } else {
        total += state->mc[i].packet[packet].data[data];
      }
    }
    return total / MC_COUNT;
  } else if (state->mc[mc].reversed) {
    return -state->mc[mc].packet[packet].data[data];
  } else {
    return state->mc[mc].packet[packet].data[data];
  }
}
