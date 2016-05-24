#include "display.h"
#include "controls.h"
#include "speed.h"
#include "mc_state.h"
#include <stdio.h>

static char display_buffer[4][20];

void display_init(const struct DisplayConfig *display) {
  lcd_init(display->lcd);
}

#pragma CODE_SECTION(display_update, ".run_from_ram")
void display_update(const struct DisplayConfig *display) {
  const char *cruise = (controls_cruise_active()) ? "CC" : "";
  const char *unit = (speed_get_unit() == SPEED_MPH) ? "mph" : "kph";
  int velocity = (int)mc_state_value(display->mc_state, MC_AVERAGE, MC_VELOCITY);
  snprintf(display_buffer[0], 20, "Speed: %3d %3s%5s", velocity, unit, cruise);
  //snprintf(display_buffer[1], 20, "");
  snprintf(display_buffer[2], 20, "Target: %3d %3s", (int)controls_cruise_target(), unit);
  snprintf(display_buffer[3], 20, "SOC: %3d%% CUR: %3d%%", 80, 40);

  lcd_println(display->lcd, LINE_0, display_buffer[0]);
  //lcd_println(display->lcd, LINE_1, display_buffer[1]);
  lcd_println(display->lcd, LINE_2, display_buffer[2]);
  lcd_println(display->lcd, LINE_3, display_buffer[3]);
}
