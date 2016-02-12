#include "mc_state.h"
#include "can/config.h"

void mc_state_init(struct MCStateConfig *state, uint16_t velocity_id, uint16_t bus_id) {
  state->packet[0].id = velocity_id;
  state->packet[1].id = bus_id;
}

void mc_state_update(struct MCStateConfig *state, struct CANMessage *msg) {
  int i;
  for (i = 0; i < 2; i++) {
    if (msg->id == state->packet[i].id) {
      state->packet[i].data_u64 = msg->data;
      break;
    }
  }
}

float *mc_state_value(struct MCStateConfig *state, MCStateValue val) {
  uint8_t packet = (val & 0x02),
          data = (val & 0x01);
  return &state->packet[packet].data[data];
}
