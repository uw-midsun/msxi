#pragma once
// control registers
// lower 8 bits
#define CTRL_LB_DSY_CHN_RDBCK_ENBL                          (1 << 0)    // D0

#define CTRL_LB_INCREM_DEVICE_ADDR                          (1 << 1)    // D1

#define CTRL_LB_LOCK_DEVICE_ADDR                            (1 << 2)    // D2

#define CTRL_LB_THRMSTR_ENBL                                (1 << 3)    // D3

#define CTRL_LB_RSVD_BIT                                    (1 << 4)    // D4

#define CTRL_LB_ACQSTN_TIME_400ns                           (0 << 5)    // D5 to D6
#define CTRL_LB_ACQSTN_TIME_800ns                           (1 << 5)    // D5 to D6
#define CTRL_LB_ACQSTN_TIME_1200ns                          (2 << 5)    // D5 to D6
#define CTRL_LB_ACQSTN_TIME_1600ns                          (3 << 5)    // D5 to D6

#define CTRL_LB_SW_RESET                                    (1 << 7)    // D7

// higher 8 bits
#define CTRL_HB_SW_POWER_DOWN                               (1 << 0)    // D8

#define CTRL_HB_CONV_AVG_1                                  (0 << 1)    // D9 to D10
#define CTRL_HB_CONV_AVG_2                                  (1 << 1)    // D9 to D10
#define CTRL_HB_CONV_AVG_4                                  (2 << 1)    // D9 to D10
#define CTRL_HB_CONV_AVG_8                                  (3 << 1)    // D9 to D10

#define CTRL_HB_CONV_CNVST                                  (0 << 3)    // D11
#define CTRL_HB_CONV_CS                                     (1 << 3)    // D11

#define CTRL_HB_CONV_RSLT_READ_ALL                          (0 << 4)    // D12 to D13
#define CTRL_HB_CONV_RSLT_READ_6CELL_AUX1_3_4               (1 << 4)    // D12 to D13
#define CTRL_HB_CONV_RSLT_READ_6CELL                        (2 << 4)    // D12 to D13
#define CTRL_HB_CONV_RSLT_READ_DSBL                         (3 << 4)    // D12 to D13

#define CTRL_HB_CONV_IN_ALL                                 (0 << 6)    // D14 to D15
#define CTRL_HB_CONV_IN_6CELL_AUX1_3_4                      (1 << 6)    // D14 to D15
#define CTRL_HB_CONV_IN_6CELL                               (2 << 6)    // D14 to D15
#define CTRL_HB_CONV_IN_SELF_TEST                           (3 << 6)    // D14 to D15

// cell balance registers
// D0 is a reserved bit
#define CB1_OUTPUT_ENBL                                     (1 << 2)    // D02
#define CB2_OUTPUT_ENBL                                     (1 << 3)    // D03
#define CB3_OUTPUT_ENBL                                     (1 << 4)    // D04
#define CB4_OUTPUT_ENBL                                     (1 << 5)    // D05
#define CB5_OUTPUT_ENBL                                     (1 << 6)    // D06
#define CB6_OUTPUT_ENBL                                     (1 << 7)    // D07


// cell timer registers (71.5s resolution, each bit is 71.5s)
// used by CBX and PD timers
#define CELL_TIMER_RSVD_BITS                                (0 << 2)
#define CELL_TIMER_71s                                      (1 << 3)
#define CELL_TIMER_142s                                     (1 << 4)
#define CELL_TIMER_286s                                     (1 << 5)
#define CELL_TIMER_572s                                     (1 << 6)
#define CELL_TIMER_1144s                                    (1 << 7)


// cnvst (conversions) control registers
// cnvst only cares about D0 and D1 (sometimes)
#define CNVST_IN_UNGATED                                      (0 << 0)
#define CNVST_IN_GATED                                        (1 << 0)
#define CNVST_IN_PULSE                                        (1 << 1)

// read register stuff
#define READ_CELL_VOLTAGE_1                                 (0x00 << 2)    // D11 to D0, read
#define READ_CELL_VOLTAGE_2                                 (0x01 << 2)    // D11 to D0, read
#define READ_CELL_VOLTAGE_3                                 (0x02 << 2)    // D11 to D0, read
#define READ_CELL_VOLTAGE_4                                 (0x03 << 2)    // D11 to D0, read
#define READ_CELL_VOLTAGE_5                                 (0x04 << 2)    // D11 to D0, read
#define READ_CELL_VOLTAGE_6                                 (0x05 << 2)    // D11 to D0, read
#define READ_AUX_ADC_1                                      (0x06 << 2)    // D11 to D0, read
#define READ_AUX_ADC_2                                      (0x07 << 2)    // D11 to D0, read
#define READ_AUX_ADC_3                                      (0x08 << 2)    // D11 to D0, read
#define READ_AUX_ADC_4                                      (0x09 << 2)    // D11 to D0, read
#define READ_AUX_ADC_5                                      (0x0A << 2)    // D11 to D0, read
#define READ_AUX_ADC_6                                      (0x0B << 2)    // D11 to D0, read
#define READ_SELF_TEST                                      (0x0C << 2)    // D11 to D0, read
#define READ_CONTROL_HB                                     (0x0D << 2)    // D15 to D8, read/write
#define READ_CONTROL_LB                                     (0x0E << 2)    // D7 to D0,  read/write
#define READ_CELL_OVERVOLTAGE                               (0x0F << 2)    // D7 to D0,  read/write
#define READ_CELL_UNDERVOLTAGE                              (0x10 << 2)    // D7 to D0,  read/write
#define READ_AUX_ADC_OVERVOLTAGE                            (0x11 << 2)    // D7 to D0,  read/write
#define READ_AUX_ADC_UNDERVOLTAGE                           (0x12 << 2)    // D7 to D0,  read/write
#define READ_ALERT                                          (0x13 << 2)    // D7 to D0,  read/write
#define READ_CELL_BALANCE                                   (0x14 << 2)    // D7 to D0,  read/write
#define READ_CB1_TIMER                                      (0x15 << 2)    // D7 to D0,  read/write
#define READ_CB2_TIMER                                      (0x16 << 2)    // D7 to D0,  read/write
#define READ_CB3_TIMER                                      (0x17 << 2)    // D7 to D0,  read/write
#define READ_CB4_TIMER                                      (0x18 << 2)    // D7 to D0,  read/write
#define READ_CB5_TIMER                                      (0x19 << 2)    // D7 to D0,  read/write
#define READ_CB6_TIMER                                      (0x1A << 2)    // D7 to D0,  read/write
#define READ_PD_TIMER                                       (0x1B << 2)    // D7 to D0,  read/write
#define READ_READ                                           (0x1C << 2)    // D7 to D0,  read/write
#define READ_CNVST_CONTROL                                  (0x1D << 2)    // D7 to D0,  read/write
