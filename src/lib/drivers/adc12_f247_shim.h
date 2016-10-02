#pragma once
// This shims the MSP430F5xxx ADC12 registers to the MSP430F247.

#if defined(__MSP430F247__)
#define ADC12SHT0_8 SHT0_8
#define ADC12ENC ENC
#define ADC12MSC MSC
#define ADC12SHP SHP
#define ADC12CONSEQ_3 CONSEQ_3
#define ADC12CONSEQ_1 CONSEQ_1
#endif
