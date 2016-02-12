#pragma once
#include "drivers/can.h"
#include "sm/event_queue.h"

// Motor controller info struct

typedef enum {
  MC_VELOCITY = 0,  // state 0 data 0
  MC_RPM,           // state 0 data 1
  MC_CURRENT,       // state 1 data 0
  MC_VOLTAGE        // state 1 data 1
} MCStateValue;

typedef enum {
  MC_LEFT,
  MC_RIGHT,
  MC_COUNT,
  MC_AVERAGE
} MotorController;

// packet[0] = Velocity, packet[1] = Bus
struct MCConfig {
  struct {
    uint16_t id;
    union {
      uint64_t data_u64;
      float data[2];
    };
  } packet[2];
};

struct MCStateConfig {
  const struct CANConfig *can;
  struct MCConfig mc[MC_COUNT];
};

// Updates the state struct with the current motor controller data if a CAN interrupt has fired
void mc_state_update(struct MCStateConfig *state, EventID e);

// Returns the current value of the requested variable
float mc_state_value(struct MCStateConfig *state, MotorController mc, MCStateValue val);
