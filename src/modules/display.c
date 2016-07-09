#include "display.h"
#include "controls.h"
#include "speed.h"
#include "mc_state.h"
#include <stdio.h>

static char display_buffer[4][20];

void display_init(const struct DisplayConfig *display) {
  lcd_init(display->lcd);
}

#include "config.h"
void display_update(const struct DisplayConfig *display) {
  //const char *cruise = (controls_cruise_active()) ? "CC" : "";
  //const char *unit = (speed_get_unit() == SPEED_MPH) ? "mph" : "kph";
  const float velocity = mc_state_value(display->mc_state, MC_LEFT, MC_VELOCITY);
  //snprintf(display_buffer[0], 20, "Speed: %3d %3s%5s", speed_convert(&velocity), unit, cruise);
  snprintf(display_buffer[0], 20, "S: %d RPM: %.3f", speed_convert(&velocity), mc_state_value(display->mc_state, MC_LEFT, MC_RPM));
//  snprintf(display_buffer[1], 20, "T: %d B: %d", adc12_sample(input.adc, input.throttle.pot.input), adc12_sample(input.adc, input.brake.regen.input));
//  snprintf(display_buffer[2], 20, "Target: %3d %3s", (int)controls_cruise_target(), unit);
  snprintf(display_buffer[3], 20, "V: %.3f A: %.3f", mc_state_value(display->mc_state, MC_LEFT, MC_VOLTAGE), mc_state_value(display->mc_state, MC_LEFT, MC_CURRENT));

  lcd_println(display->lcd, LINE_0, display_buffer[0]);
  //lcd_println(display->lcd, LINE_1, display_buffer[1]);
  //lcd_println(display->lcd, LINE_2, display_buffer[2]);
  lcd_println(display->lcd, LINE_3, display_buffer[3]);
}
