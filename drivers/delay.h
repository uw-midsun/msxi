#pragma once
#include "stdint.h"

// Timer_A0-based delay
// Enters LPM3 for the specified period of time. (Blocking)

// This function _may_ return early if another interrupt that exits sleep mode fires first.
// This is generally not a problem, since the only ISR that does that is the SPI ISR.
void delay_seconds(uint8_t seconds);
