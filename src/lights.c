#include <msp430.h>
#include <stdlib.h>
#include "lights.h"
#include "config.h"
#include "can/config.h"
#define BLINK_CYCLES 65535

enum LightState {
  LIGHT_LEFT_TURN,
  LIGHT_RIGHT_TURN,
  LIGHT_HAZARD,
  LIGHT_RUNNING,
};

static enum LightState current_state = LIGHT_RUNNING;

static void prv_start_timer() {
  // Start timer that counts to BLINK_CYCLES
  TACTL |= TASSEL_2 | MC_2 | ID_3;
  TACCTL0 |= CCIE;
  TACCR0 = BLINK_CYCLES;
}

static void prv_stop_timer() {
  TACTL = MC_0;
  TACTL = TACLR;
}

/* Read message type and data from msg and enable/disable
 * the apporitate lights and update the current state if
 * required.
 */
static void prv_handle_message(struct CANMessage *msg) {
  IOState state = msg->data == 0 ? IO_LOW : IO_HIGH;
  uint8_t i;
  switch(msg->id) {
    case THEMIS_SIG_LEFT:
      current_state = msg->data == 0 ? LIGHT_RUNNING : LIGHT_LEFT_TURN;
      for(i=0; i < MAX_LIGHTS; ++i) {
        io_set_state(&left_turn_lights[i], state);
        
        // Turn on brake signals
        io_set_state(&left_brake_signal[i], state);
        io_set_state(&brake_lights[i], state);
      }
      break;
    case THEMIS_SIG_RIGHT:
      current_state = msg->data == 0 ? LIGHT_RUNNING : LIGHT_RIGHT_TURN;
      for(i=0; i < MAX_LIGHTS; ++i) {
        io_set_state(&right_turn_lights[i], state);
        
        // Turn on brake signals
        io_set_state(&right_brake_signal[i], state);
        io_set_state(&brake_lights[i], state);
      }
      break;
    case THEMIS_SIG_HAZARD:
      current_state = msg->data == 0 ? LIGHT_RUNNING : LIGHT_HAZARD;
      for(i=0; i < MAX_LIGHTS; ++i) {
        io_set_state(&left_turn_lights[i], state);
        io_set_state(&left_turn_lights[i], state);
        
        // Turn on brake signals
        io_set_state(&right_turn_lights[i], state);
        io_set_state(&right_brake_signal[i], state);
        io_set_state(&brake_lights[i], state);
      }
      break;
    case THEMIS_SIG_BRAKE:
      for(i=0; i < MAX_LIGHTS; ++i) {
        io_set_state(&brake_lights[i], state);
        
        // Turn signals take priority over braking
        // It's in the regulations
        if(current_state != LIGHT_LEFT_TURN) {
          io_set_state(&left_brake_signal[i], state);
        }
        if(current_state != LIGHT_RIGHT_TURN) {
          io_set_state(&right_brake_signal[i], state);
        }
      }
      break;
  }
}

void lights_init() {
  uint8_t i;
  
  // Initalize all inital light states
  for(i=0; i < MAX_LIGHTS; ++i) {
    io_set_dir(&brake_lights[i], PIN_OUT);
    io_set_dir(&left_turn_lights[i], PIN_OUT);
    io_set_dir(&right_turn_lights[i], PIN_OUT);
    io_set_dir(&running_lights[i], PIN_OUT);
    io_set_dir(&left_brake_signal[i], PIN_OUT);
    io_set_dir(&right_brake_signal[i], PIN_OUT);
    
    io_set_state(&running_lights[i], IO_HIGH);
  }
  
  io_set_dir(&can_interrupt, PIN_IN);
  
  can_init(&can);

  // Start timer that is used for light blinking
  prv_start_timer();
}

void lights_process_message(void) {
  // Check interrupt pin for active low to see if
  // if we have a message.

  if(io_get_state(&can.interrupt_pin) == IO_LOW) {
    struct CANMessage msg = {0};
    struct CANError error = {0};
    while(can_process_interrupt(can_cfg, &msg, &error)) {
      prv_handle_message(&msg);
    }
  }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_A0_ISR(void) {
  uint8_t i;
  // Blink the appropriate lights based on the state
  switch(current_state) {
    case LIGHT_LEFT_TURN:
      for(i = 0; i < MAX_LIGHTS; ++i) {
        io_toggle(&left_turn_lights[i]);
        io_toggle(&left_brake_signal[i]);
      }
      break;
    case LIGHT_RIGHT_TURN:
      for(i = 0; i < MAX_LIGHTS; ++i) {
        io_toggle(&right_turn_lights[i]);
        io_toggle(&right_brake_signal[i]);
      }
      break;
    case LIGHT_HAZARD:
      for(i = 0; i < MAX_LIGHTS; ++i) {
        io_toggle(&left_turn_lights[i]);
        io_toggle(&right_turn_lights[i]);
        
        io_toggle(&left_brake_signal[i]);
        io_toggle(&left_brake_signal[i]);
      }
      break;
  }
}
