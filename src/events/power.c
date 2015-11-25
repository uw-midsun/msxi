#include "power.h"
#include "drivers/delay.h"
#include "sm/event_queue.h"

#define MAX_ATTEMPTS 1
#define WAIT_SECONDS 1

static bool prv_poll_status(const struct PowerConfig *pwr) {
  uint16_t voltage = adc12_sample(pwr->adc, pwr->measure);

  // Within bounds
  return (voltage >= pwr->target.low) && (voltage <= pwr->target.high);
}

void power_status_check(void *dcdc) {
  struct PowerConfig *pwr = (struct PowerConfig *)dcdc;

  uint8_t tries = 0;
  while (tries <= MAX_ATTEMPTS) {
    if (prv_poll_status(pwr)) {
      // DC-DC voltage is good
      event_raise(POWER_GOOD, 0);
      return;
    }
    tries++;
    delay_seconds(WAIT_SECONDS);
  }

  // After waiting, it still isn't good - something must be wrong.
  // TODO: should this be moved into the state machine? We can't handle the killswitch within 1s.
  event_raise(POWER_BAD, 0);
}
