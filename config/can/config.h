#pragma once

// CAN message map
// Devices are prepended with DEVICE_ (ex. DEVICE_CHAOS)
// Messages are prepended with [device]_ (ex. CHAOS_STATE_CHANGE)

#define CAN_IGNORE_ALL 0x0
#define CAN_DEVICE_MASK 0x7E0
#define CAN_MESSAGE_MASK 0x1F
#define CAN_FULL_MASK (CAN_DEVICE_MASK | CAN_MESSAGE_MASK)

#define DEVICE(name, id) DEVICE_##name = ((id) << 5),
typedef enum {
#include "devices.inc"
} DeviceID;
#undef DEVICE

#define _CAN(device, name, id, format) device##_##name = ((DEVICE_##device) | (id)),

#define CAN(name, id, format) _CAN(THEMIS, name, id, format)
typedef enum {
#include "themis.inc"
} ThemisMessageID;
#undef CAN

#define CAN(name, id, format) _CAN(CHAOS, name, id, format)
typedef enum {
#include "chaos.inc"
} ChaosMessageID;
#undef CAN

#define CAN(name, id, format) _CAN(LEFT_MC, name, id, format)
typedef enum {
#include "motor_controller.inc"
} LeftMCMessageID;
#undef CAN

#define CAN(name, id, format) _CAN(RIGHT_MC, name, id, format)
typedef enum {
#include "motor_controller.inc"
} RightMCMessageID;
#undef CAN

#define CAN(name, id, format) _CAN(PLUTUS, name, id, format)
typedef enum {
#include "plutus.inc"
} PlutusMessageId;
#undef CAN

#undef _CAN
