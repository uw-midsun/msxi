#pragma once

// Contains relevant MCP2515 register data

// SPI commands: Table 12-1
#define MCP_RESET       0xC0
#define MCP_READ        0x03
#define MCP_READ_RX     0x90
#define MCP_WRITE       0x02
#define MCP_LOAD_TX     0x40
#define MCP_RTS         0x80
#define MCP_READ_STATUS 0xA0
#define MCP_RX_STATUS   0xB0
#define MCP_BIT_MODIFY  0x05

// READ_RX Arguments: Figure 12-3
#define READ_RXB0SIDH   0x00 // ID
#define READ_RXB0D0     0x02 // Data
#define READ_RXB1SIDH   0x04
#define READ_RXB1D0     0x06

// LOAD_TX Arguments: Figure 12-5
#define LOAD_TXB0SIDH   0x00
#define LOAD_TXB0D0     0x01
#define LOAD_TXB1SIDH   0x02
#define LOAD_TXB1D0     0x03
#define LOAD_TXB2SIDH   0x04
#define LOAD_TXB2D0     0x05

// RTS Arguments: Table 12-1 (RTS)
#define RTS_TXB0        0x01
#define RTS_TXB1        0x02
#define RTS_TXB2        0x04

// READ_STATUS Response: Figure 12-8
#define STATUS_RX0IF    0x01
#define STATUS_RX1IF    0x02
#define STATUS_TX0REQ   0x04
#define STATUS_TX0IF    0x08
#define STATUS_TX1REQ   0x10
#define STATUS_TX1IF    0x20
#define STATUS_TX2REQ   0x40
#define STATUS_TX2IF    0x80

// Control Registers: Table 11-2
#define BFPCTRL         0x0C // RX pins disabled by default
#define TXRTSCTRL       0x0D // TX pins input by default
#define CANSTAT         0x0E
#define CANCTRL         0x0F
#define TEC             0x1C
#define REC             0x1D
#define CNF3            0x28
#define CNF2            0x29
#define CNF1            0x2A
#define CANINTE         0x2B
#define CANINTF         0x2C
#define EFLG            0x2D
#define TXB0CTRL        0x30
#define TXB1CTRL        0x40
#define TXB2CTRL        0x50
#define RXB0CTRL        0x60
#define RXB1CTRL        0x70

// Filter/Mask Registers: Table 11-1
#define RXF0SIDH        0x00
#define RXF1SIDH        0x04
#define RXF2SIDH        0x08
#define RXF3SIDH        0x10
#define RXF4SIDH        0x14
#define RXF5SIDH        0x18
#define RXM0SIDH        0x20
#define RXM1SIDH        0x24

// CANCTRL: Register 10-2
#define OPMODE_MASK     0xE0
#define OPMODE_NORMAL   0x00
#define OPMODE_SLEEP    0x20
#define OPMODE_LOOPBACK 0x40
#define OPMODE_LISTEN   0x60
#define OPMODE_CONFIG   0x80

#define CLKOUT_MASK     0x07
#define CLKPRE_1        0x04 // CLKEN is automatically enabled
#define CLKPRE_2        0x05
#define CLKPRE_4        0x06
#define CLKPRE_8        0x07

// CNF3: Register 5-3
#define PHSEG2_MASK     0x07

// CNF2: Register 5-2
#define BTLMODE_MASK    0x80
#define BTLMODE_CNF3    0x80

#define SAMPLE_MASK     0x40
#define SAMPLE_3X       0x40

#define PHSEG1_MASK     0x38
#define PRSEG_MASK      0x07

// CNF1: Register 5-1
#define BRP_MASK        0x3F

// CANINTE/INTF: Register 7-1/2
#define MSG_ERROR       0x80
#define WAKEUP          0x40
#define EFLAG           0x20
#define TX2IE           0x10
#define TX1IE           0x08
#define TX0IE           0x04
#define RX1IE           0x02
#define RX0IE           0x01

// EFLG: Register 6-3
#define RX1_OVERFLOW    0x80
#define RX0_OVERFLOW    0x40
#define TX_BUS_OFF      0x20
#define TX_EP_FLAG      0x10
#define RX_EP_FLAG      0x08
#define TX_WARNING      0x04
#define RX_WARNING      0x02
#define ERROR_WARNING   0x01

// TXBnDLC: Register 3-7
#define RTR_SHIFT       6
#define RTR_FRAME       0x40
