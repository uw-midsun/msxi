#pragma once

// CAN message map
// Devices are prepended with DEVICE_ (ex. DEVICE_CHAOS)
// Messages are prepended with [device]_ (ex. CHAOS_STATE_CHANGE)

#define CAN_DEVICE_MASK 0x7E0
#define CAN_MESSAGE_MASK 0x1F
#define CAN_FULL_MASK (CAN_DEVICE_MASK | CAN_MESSAGE_MASK)

#define DEVICE(name, id) DEVICE_##name = ((id) << 5),
typedef enum {
#include "can/devices.inc"
} DeviceID;
#undef DEVICE

#define _CAN(device, name, id, format) device##_##name = ((DEVICE_##device) | (id)),
#define CAN(name, id, format) _CAN(THEMIS, name, id, format)
typedef enum {
#include "can/themis.inc"
} ThemisMessageID;
#undef CAN

#define CAN(name, id, format) _CAN(CHAOS, name, id, format)
typedef enum {
#include "can/chaos.inc"
} ChaosMessageID;
#undef CAN

#undef _CAN
