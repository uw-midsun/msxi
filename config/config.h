#pragma once
#include "drivers/afe.h"
#include "drivers/gpio_map.h"
#include "drivers/adc12.h"
#include "drivers/can.h"
#include "drivers/current_sensor.h"
#include "wdt_a.h"
#include "can/config.h"

extern const struct IOMap heartbeat;
extern const struct IOMap spst;
extern const struct IOMap convst;
extern const struct IOMap afe_pd;
extern const struct IOMap alert;
extern const struct IOMap pwm_rising;
extern const struct IOMap pwm_falling;

extern const struct IOMap rail_5000mV;
extern const struct IOMap rail_3300mV;

extern struct IOMap leds[8];

extern struct AFEConfig afe;
extern const struct ADC12Config adc12;
extern const struct PWMConfig pwm;
extern const struct CANConfig can;

extern const struct Threshold rail_thresh_5000mV;
extern const struct Threshold rail_thresh_3300mV;

extern const struct TempThreshold discharge_threshold;
extern const struct TempThreshold charge_threshold;

extern const struct Threshold bal1;  // range1: [0, 4000mV)
extern const struct Threshold bal2;  // range2: [4000mV, 4150mV)
extern const struct Threshold bal3;  // range3: [4150mV, 4200mV)
extern const struct Threshold danger_threshold; // extremely dangerous
