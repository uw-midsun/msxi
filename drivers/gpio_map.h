#pragma once

// GPIO header replacement since the F247 doesn't support driverlib

#define GPIO_PORT_P1    0x01
#define GPIO_PORT_P2    0x02
#define GPIO_PORT_P3    0x03
#define GPIO_PORT_P4    0x04
#define GPIO_PORT_P5    0x05
#define GPIO_PORT_P6    0x06
#define GPIO_PORT_P7    0x07
#define GPIO_PORT_P8    0x08
#define GPIO_PORT_P9    0x09
#define GPIO_PORT_P10   0x0A

#define GPIO_PIN0       (0x01)
#define GPIO_PIN1       (0x02)
#define GPIO_PIN2       (0x04)
#define GPIO_PIN3       (0x08)
#define GPIO_PIN4       (0x10)
#define GPIO_PIN5       (0x20)
#define GPIO_PIN6       (0x40)
#define GPIO_PIN7       (0x80)
