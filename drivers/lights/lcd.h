#pragma once
#include "drivers/io_map.h"

// LCD Driver (ST7066)

struct LCDConfig {
  struct IOMap backlight;
  struct IOMap rs; // Register select - 0: command, 1: data
  struct IOMap rw; // Read/Write - 0: Write, 1: Read
  struct IOMap enable;
  struct IOMap data; // Px.y = LCD_Dy; use { GPIO_PORT_Px, GPIO_PIN_ALL }
};

// DDRAM offsets for a 20x4 display
typedef enum {
  LINE_0 = 0,
  LINE_1 = 64,
  LINE_2 = 20,
  LINE_3 = 84
} LCDLine;

void lcd_init(struct LCDConfig *lcd);

// Prints the 20-character line on the specified line
void lcd_println(const struct LCDConfig *lcd, LCDLine line, char *data);
