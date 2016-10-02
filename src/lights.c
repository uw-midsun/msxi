#include <msp430.h>
#include <stdlib.h>
#include "lights.h"
#include "can/config.h"
#define BLINK_CYCLES 65535

static struct LightConfig *config;

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
  uint16_t i, j;
  bool update_brakes = false;

  for (i = 0; i < MAX_SIGNALS; ++i) {
    if (config->signals[i].can_msg_id == msg->id) {
      for (j = 0; j < MAX_LIGHTS; ++j) {
        io_set_state(&config->signals[i].signal_pins[j], state);
      }

      if(state == IO_LOW) {
        /*
         * When a blinking light turns off we set a flag to update the brake lights
         * The is required for MSXI since the brake lights are also the rear turn
         * signals for the car.
         */
        if(config->signals[i].type == LIGHT_BLINK) {
          update_brakes = true;
        }
        config->signal_states &= ~(1 << i);
      } else {
        config->signal_states |= (1 << i);
      }
    }
  }

  /*
   * If we have been told update the brakes, go through all signals in the
   * lights config and find any brakes that are supposed to be on right now
   * and turn the back on so that they stay on when the turn signal turns
   * off.
   */
  if (update_brakes) {
    for (i = 0; i < MAX_SIGNALS; ++i) {
      bool signal_state = (config->signal_states >> i) & 0x1;
      if (config->signals[i].type == LIGHT_BRAKE && signal_state) {
        for (j = 0; j < MAX_LIGHTS; ++j) {
          io_set_state(&config->signals[i].signal_pins[j], IO_HIGH);
        }
      }
    }
  }
}

void lights_init(struct LightConfig *light_config) {
  uint16_t i, j;
  
  config = light_config;

  // Initalize all inital light states
  for (i = 0; i < MAX_SIGNALS; ++i) {
    for (j = 0; j < MAX_LIGHTS; ++j) {
    	io_set_state(&config->signals[i].signal_pins[j], IO_LOW);
    	io_set_dir(&config->signals[i].signal_pins[j], PIN_OUT);
    }
  }

  can_init(&config->can);

  // Start timer that is used for light blinking
  prv_start_timer();
}

void lights_process_message(void) {
  // Check interrupt pin for active low to see if
  // if we have a message.

  if (!(P2IN & BIT7)) {
    struct CANMessage msg = {0};
    can_receive(&config->can, &msg);
    if (msg.id != 0x00) {
      prv_handle_message(&msg);
    }
  }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_A0_ISR(void) {
  uint16_t i, j;
  // Blink the appropriate lights based on the state
  config->active = !config->active;
  for (i = 0; i < MAX_SIGNALS; ++i) {
    bool signal_state = (config->signal_states >> i) & 0x1;
    // If this light blinks and light is on
    if (config->signals[i].type == LIGHT_BLINK && signal_state) {
      for (j = 0; j < MAX_LIGHTS; ++j) {
        io_set_state(&config->signals[i].signal_pins[j], config->active);
      }
    }
  }
}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
  io_process_interrupt(&config->can.interrupt_pin);
}
