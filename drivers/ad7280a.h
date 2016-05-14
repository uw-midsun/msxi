#pragma once

// register name                                    value           position in message, read/write
#define AFE_SELF_TEST                               0x0C            // D11 to D0, read

#define AFE_CONTROL_HB                              0x0D            // D15 to D8, read/write
#define AFE_CONTROL_LB                              0x0E            // D7 to D0,  read/write

#define AFE_CELL_OVERVOLTAGE                        0x0F            // D7 to D0,  read/write
#define AFE_CELL_UNDERVOLTAGE                       0x10            // D7 to D0,  read/write

#define AFE_AUX_ADC_OVERVOLTAGE                     0x11            // D7 to D0,  read/write
#define AFE_AUX_ADC_UNDERVOLTAGE                    0x12            // D7 to D0,  read/write

#define AFE_ALERT                                   0x13            // D7 to D0,  read/write

#define AFE_CELL_BALANCE                            0x14            // D7 to D0,  read/write

#define AFE_PD_TIMER                                0x1B            // D7 to D0,  read/write

#define AFE_READ                                    0x1C            // D7 to D0,  read/write

#define AFE_CNVST_CONTROL                           0x1D            // D7 to D0,  read/write

// control registers
#define CTRL_LB_DAISY_CHAIN_RB_ENB                  (1 << 0)    // D0

#define CTRL_LB_INCREM_DEVICE_ADDR                  (1 << 1)    // D1

#define CTRL_LB_LOCK_DEVICE_ADDR                    (1 << 2)    // D2

#define CTRL_LB_THRMSTR_ENBL                        (1 << 3)    // D3

#define CTRL_LB_RSVD_BIT                            (1 << 4)    // D4

#define CTRL_LB_ACQSTN_TIME_400ns                   (0 << 5)    // D5 to D6
#define CTRL_LB_ACQSTN_TIME_800ns                   (1 << 5)    // D5 to D6
#define CTRL_LB_ACQSTN_TIME_1200ns                  (2 << 5)    // D5 to D6
#define CTRL_LB_ACQSTN_TIME_1600ns                  (3 << 5)    // D5 to D6

#define CTRL_LB_SW_RESET                            (1 << 7)    // D7

// higher 8 bits
#define CTRL_HB_SW_POWER_DOWN                       (1 << 0)    // D8

#define CTRL_HB_CONV_AVG_1                          (0 << 1)    // D9 to D10
#define CTRL_HB_CONV_AVG_2                          (1 << 1)    // D9 to D10
#define CTRL_HB_CONV_AVG_4                          (2 << 1)    // D9 to D10
#define CTRL_HB_CONV_AVG_8                          (3 << 1)    // D9 to D10

#define CTRL_HB_CONV_CNVST                          (0 << 3)    // D11
#define CTRL_HB_CONV_CS                             (1 << 3)    // D11

#define CTRL_HB_CONV_RSLT_READ_ALL                  (0 << 4)    // D12 to D13
#define CTRL_HB_CONV_RSLT_READ_6CELL_AUX1_3_4       (1 << 4)    // D12 to D13
#define CTRL_HB_CONV_RSLT_READ_6CELL                (2 << 4)    // D12 to D13
#define CTRL_HB_CONV_RSLT_READ_DSBL                 (3 << 4)    // D12 to D13

#define CTRL_HB_CONV_INPUT_ALL                      (0 << 6)    // D14 to D15
#define CTRL_HB_CONV_INPUT_6CELL_AUX1_3_4           (1 << 6)    // D14 to D15
#define CTRL_HB_CONV_INPUT_6CELL                    (2 << 6)    // D14 to D15
#define CTRL_HB_CONV_INPUT_SELF_TEST                (3 << 6)    // D14 to D15


// cnvst (conversions) control registers
// cnvst only cares about D0 and D1 (sometimes)
#define CNVST_IN_UNGATED                            (0 << 0)
#define CNVST_IN_GATED                              (1 << 0)
#define CNVST_IN_PULSE                              (1 << 1)


// read register values
#define READ_CELL_VOLTAGE_1                         (AFE_CELL_VOLTAGE_1 << 2)   // D11 to D0, read
#define READ_CELL_VOLTAGE_2                         (AFE_CELL_VOLTAGE_2 << 2)   // D11 to D0, read
#define READ_CELL_VOLTAGE_3                         (AFE_CELL_VOLTAGE_3 << 2)   // D11 to D0, read
#define READ_CELL_VOLTAGE_4                         (AFE_CELL_VOLTAGE_4 << 2)   // D11 to D0, read
#define READ_CELL_VOLTAGE_5                         (AFE_CELL_VOLTAGE_5 << 2)   // D11 to D0, read
#define READ_CELL_VOLTAGE_6                         (AFE_CELL_VOLTAGE_6 << 2)   // D11 to D0, read

#define READ_AUX_ADC_1                              (AFE_AUX_ADC_1 << 2)        // D11 to D0, read
#define READ_AUX_ADC_2                              (AFE_AUX_ADC_2 << 2)        // D11 to D0, read
#define READ_AUX_ADC_3                              (AFE_AUX_ADC_3 << 2)        // D11 to D0, read
#define READ_AUX_ADC_4                              (AFE_AUX_ADC_4 << 2)        // D11 to D0, read
#define READ_AUX_ADC_5                              (AFE_AUX_ADC_5 << 2)        // D11 to D0, read
#define READ_AUX_ADC_6                              (AFE_AUX_ADC_6 << 2)        // D11 to D0, read

#define READ_SELF_TEST                              (AFE_SELF_TEST << 2)        // D11 to D0, read

#define READ_CONTROL_HB                             (AFE_CONTROL_HB << 2)       // D15 to D8, read/write
#define READ_CONTROL_LB                             (AFE_CONTROL_LB << 2)       // D7 to D0,  read/write

#define READ_CELL_OVERVOLTAGE                       (AFE_CELL_OVERVOLTAGE << 2)    // D7 to D0,  read/write
#define READ_CELL_UNDERVOLTAGE                      (AFE_CELL_UNDERVOLTAGE << 2)    // D7 to D0,  read/write

#define READ_AUX_ADC_OVERVOLTAGE                    (AFE_AUX_ADC_OVERVOLTAGE << 2)    // D7 to D0,  read/write
#define READ_AUX_ADC_UNDERVOLTAGE                   (AFE_AUX_ADC_UNDERVOLTAGE << 2)    // D7 to D0,  read/write

#define READ_ALERT                                  (AFE_ALERT << 2)    // D7 to D0,  read/write

#define READ_CELL_BALANCE                           (AFE_CELL_BALANCE << 2)    // D7 to D0,  read/write

#define READ_CB1_TIMER                              (AFE_CB1_TIMER << 2)    // D7 to D0,  read/write
#define READ_CB2_TIMER                              (AFE_CB2_TIMER << 2)    // D7 to D0,  read/write
#define READ_CB3_TIMER                              (AFE_CB3_TIMER << 2)    // D7 to D0,  read/write
#define READ_CB4_TIMER                              (AFE_CB4_TIMER << 2)    // D7 to D0,  read/write
#define READ_CB5_TIMER                              (AFE_CB5_TIMER << 2)    // D7 to D0,  read/write
#define READ_CB6_TIMER                              (AFE_CB6_TIMER << 2)    // D7 to D0,  read/write

#define READ_PD_TIMER                               (AFE_PD_TIMER << 2)    // D7 to D0,  read/write

#define READ_READ                                   (AFE_READ << 2)    // D7 to D0,  read/write

#define READ_CNVST_CONTROL                          (AFE_CNVST_CONTROL << 2)    // D7 to D0,  read/write

// number of bits that CRC-8 uses when computing a checksum
#define AFE_CRC_NUMBITS_READ                        22              // CRC bits,  read
#define AFE_CRC_NUMBITS_WRITE                       21              // CRC bits,  write


// CRC-8 bits
//   P(x) = x^8 + x^5 + x^3 + x^2 + x^1 + x^0
//        = 0b100101111
//        = 0x2F
#define AFE_CRC_POLYNOMIAL_ORDER                    8
#define AFE_CRC_POLYNOMIAL                          0x2F
#define AFE_HIGHBIT                                 (1 << (AFE_CRC_POLYNOMIAL_ORDER - 1))


// misc bits
#define AFE_WRITE_BIT_PATTERN                       0x2             // required Bits[D2:D0]
#define AFE_ADC_12_BIT_RES                          4095            // AD7280A uses a 12 bit ADC

#define AFE_READ_TRANSFER_VALUE                     0xF800030A

// SPI configuration
// recommended SCLK to ensure correct operation of daisy-chain interface is 1MHz
//   when reading back from a single device, the SCLK must be lower than 1 MHz
//   to read back register data up the chain
#define AFE_SPI_MAX_CLK                             1000000         // clock speed in Hz
#define AFE_MAX_DAISY_CHAIN                         8               // we can have maximum 8 devices in daisy chain
