#pragma once
#include "drivers/can.h"

// Motor controller info struct

typedef enum {
  MC_VELOCITY = 0,  // state 0 data 0
  MC_RPM,           // state 0 data 1
  MC_CURRENT,       // state 1 data 0
  MC_VOLTAGE        // state 1 data 1
} MCStateValue;

// Do not configure this manually - use mc_state_init
struct MCStateConfig {
  struct {
    uint16_t id;
    union {
      uint64_t data_u64;
      float data[2];
    };
  } packet[2];
};

// Initializes the state config so packet IDs match
void mc_state_init(struct MCStateConfig *state, uint16_t velocity_id, uint16_t bus_id);

// Updates the state struct with the current motor controller data
void mc_state_update(struct MCStateConfig *state, struct CANMessage *msg);

// Returns a pointer to the current value of the requested variable
float *mc_state_value(struct MCStateConfig *state, MCStateValue val);
