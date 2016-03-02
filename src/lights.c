#include <stdlib.h>
#include "lights.h"
#include "lights_config.h"

static const struct CANConfig *can_cfg = NULL;

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
}

void lights_process_message(void) {
  // Check interrupt pin for active low to see if
  // if we have a message.
  if(io_get_state(&can_interrupt) == IO_LOW) {
    struct CANMessage msg = {0};
    struct CANError error = {0};
    while(can_process_)
  }
}
