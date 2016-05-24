#include <drivers/themis/lcd.h>
#include <drivers/themis/st7066.h>

static void prv_clock(const struct LCDConfig *lcd) {
  io_set_state(&lcd->enable, IO_HIGH);
  __delay_cycles(500);
  io_set_state(&lcd->enable, IO_LOW);
}


static void prv_command(const struct LCDConfig *lcd, uint8_t command) {
  io_set_state(&lcd->rs, IO_LOW);
  io_set_state(&lcd->rw, IO_LOW);

  io_set_port(&lcd->data, command);

  prv_clock(lcd);
}

static void prv_write(const struct LCDConfig *lcd, uint8_t character) {
  io_set_state(&lcd->rs, IO_HIGH);
  io_set_state(&lcd->rw, IO_LOW);

  io_set_port(&lcd->data, character);

  prv_clock(lcd);
}

void lcd_init(const struct LCDConfig *lcd) {
  const struct IOMap *io[5] = { &lcd->backlight, &lcd->rs, &lcd->rw, &lcd->enable, &lcd->data };
  uint16_t i;
  for (i = 0; i < 5; i++) {
    io_set_state(io[i], IO_LOW);
    io_set_dir(io[i], PIN_OUT);
  }

  io_set_state(&lcd->backlight, IO_HIGH);

  __delay_cycles(15000); // ~15ms? Consider switching to timer
  prv_command(lcd, FUNCTION_SET | FUNC_8BIT); // Wakeup 1
  __delay_cycles(3000); // ~3ms?
  prv_command(lcd, FUNCTION_SET | FUNC_8BIT); // Wakeup 2
  __delay_cycles(160); // ~160us?
  prv_command(lcd, FUNCTION_SET | FUNC_8BIT); // Wakeup 3
  __delay_cycles(160);
  prv_command(lcd, FUNCTION_SET | FUNC_8BIT | FUNC_2LINE); // Set interface (actually 4x20)
  prv_command(lcd, DISPLAY_CTRL | DISPLAY_ON);
  prv_command(lcd, ENTRY_MODE | CURSOR_INC); // Increment the cursor
  prv_command(lcd, RETURN_HOME);
  prv_command(lcd, CLEAR_DISPLAY);
}

// Prints the 20-character line on the specified line
void lcd_println(const struct LCDConfig *lcd, LCDLine line, char *data) {
  prv_command(lcd, DDRAM_SET | line); // Move to line

  uint16_t i;
  for (i = 0; i < 20 && data[i] != '\0'; i++) {
    prv_write(lcd, data[i]);
  }
}
