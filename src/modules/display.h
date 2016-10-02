#pragma once
#include "drivers/themis/lcd.h"
#include "mc_state.h"

// LCD display - shows speed and power info

struct DisplayConfig {
  const struct LCDConfig *lcd;
  struct MCStateConfig *mc_state;
};

void display_init(const struct DisplayConfig *display);

void display_update(const struct DisplayConfig *display);
