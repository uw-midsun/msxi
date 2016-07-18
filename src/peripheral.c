#include "peripheral.h"
#include "msp430.h"
#include "config.h"
#include "drivers/can.h"
#include "drivers/adc12.h"
#include "drivers/relay.h"
#include "drivers/timer.h"
#include "sm/sm_debug.h"
#include "events/horn.h"
#include "events/fail.h"
#include "events/power.h"
#include "events/input.h"
#include "events/heartbeat.h"
#include "events/motor_controller.h"

void peripheral_init(void) {
  power_set_lv(&enable_lv, LV_DISABLED);

  can_init(&can);

  fail_init(&can);
  sm_debug_init(&sm_debug);
  horn_init(&can, &horn);

  input_init(&input);
  heartbeat_init(&plutus_heartbeat);

  adc12_init(&adc12_a);
  mc_init(&mc_config);

  relay_init(&relay_battery);
  relay_init(&relay_solar);

  timer_init();

  timer_delay_periodic(40, input_poll, &input);
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void) {
  // Process horn (CAN) messages
  horn_interrupt();
  heartbeat_interrupt();
}
