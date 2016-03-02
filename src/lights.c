#include <stdlib.h>
#include "lights.h"
#include "lights_config.h"

enum LightState {
  LIGHT_LEFT_TURN,
  LIGHT_RIGHT_TURN,
  LIGHT_HAZARD,
  LIGHT_RUNNING,
};

static const struct CANConfig *can_cfg = NULL;
static enum LightState current_state = LIGHT_RUNNING; 

static void prv_handle_message(struct CANMessage *msg) {
  IOState state = msg->data == 0 ? IO_LOW : IO_HIGH;
  switch(msg->id) {
    case THEMIS_SIG_LEFT:
      current_state = msg->data == 0 ? LIGHTS_RUNNNG : LIGHTS_LEFT_TURN;
      break;
    case THEMIS_SIG_RIGHT:
      current_state = msg->data == 0 ? LIGHTS_RUNNING : LIGHTS_RIGHT_TURN;
      break;
    case THEMIS_SIG_HAZARD:
      current_state = msg->data == 0 ? LIGHT_RUNNING : LIGHTS_HAZARD;
      break;
    case THEMIS_SIG_BRAKE:
      for(int i=0; i < MAX_LIGHTS; ++i) {
        io_set_state(&brake_lights[i], state);
      }
      break;
  }
}

void lights_init(const struct CANConfig *can) {
  can_cfg = can;
  
  for(int i=0; i < MAX_LIGHTS; ++i) {
    io_set_dir(&brake_lights[i], PIN_OUT);
    io_set_dir(&light_turn_lights[i], PIN_OUT);
    io_set_dir(&right_turn_lights[i], PIN_OUT);
    io_set_dir(&running_lights[i], PIN_OUT);
    
    io_set_state(&running_lights[i], IO_HIGHT);
  }
  
  io_set_dir(&can_interrupt, PIN_IN);
  
  can_init(can_cfg);

  // Set up timer
  TACTL |= TASSEL_2 | MC_2 | ID_3;
}

void lights_process_message(void) {
  // Check interrupt pin for active low to see if
  // if we have a message.
  if(io_get_state(&can_interrupt) == IO_LOW) {
    struct CANMessage msg = {0};
    struct CANError error = {0};
    while(can_process_interrupt(can_cfg, &msg, &error)) {
      prv_handle_message(&msg);
    }
  }
}
