#pragma once

// ST7066 instructions

// Command bases: RS = 0, RW = 0
#define CLEAR_DISPLAY 0x01
#define RETURN_HOME   0x02
#define ENTRY_MODE    0x04
#define DISPLAY_CTRL  0x08
#define CURSOR_CTRL   0x10
#define FUNCTION_SET  0x20
#define CGRAM_SET     0x40
#define DDRAM_SET     0x80

// Entry mode
#define CURSOR_INC    0x02
#define ENTRY_SHIFT   0x01

// Display control
#define DISPLAY_ON    0x04
#define CURSOR_ON     0x02
#define BLINK_ON      0x01

// Function set
#define FUNC_8BIT     0x10
#define FUNC_2LINE    0x08
#define FONT_5X10     0x04
