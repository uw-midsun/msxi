#include <msp430.h>
#include <stdbool.h>
#include "lights.h"
#include "config.h"


int main(void) {
  // Disable watchdog timer for now
  // TODO Actually use watchdog for what it's designed for
  WDTCTL = WDTPW | WDTHOLD;
  
  lights_init();
  
  while(true) {
    lights_process_message();
  }
  return 0;
}
