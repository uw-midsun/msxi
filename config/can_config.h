#pragma once

// CAN message map

#define CAN_DEVICE_MASK 0x7E0
#define CAN_MESSAGE_MASK 0x1F
#define CAN_FULL_MASK (CAN_DEVICE_MASK | CAN_MESSAGE_MASK)
#define CAN_DEVICE_ID(device) ((device) << 5)
#define CAN_ID(device, message) (CAN_DEVICE_ID(device) | (message))
#define CAN_FAIL_CODE(gen, sub) ((gen) << 32 | (sub))

typedef enum {
  DEVICE_CHAOS = 0,
  DEVICE_PLUTUS,
  DEVICE_THEMIS,
  DEVICE_LIGHTS,
  DEVICE_TELEMETRY,
  DEVICE_LEFT_MC,
  DEVICE_RIGHT_MC,
  DEVICE_UNUSED = CAN_FULL_MASK // Used to force max data type
} DeviceID;

typedef enum {
  CHAOS_STATE_CHANGE = CAN_ID(DEVICE_CHAOS, 0),
  CHAOS_FAIL
} ChaosMessageID;

// Failure codes use the data_u32 type.
// [1] = General failure code, [0] = Sub-failure code
typedef enum {
  FAIL_RELAY = 0,
  FAIL_KILLSWITCH,
  FAIL_HEARTBEAT_BAD,
  FAIL_DCDC_BAD,
  FAIL_MC_BAD
} ChaosFailCode;

typedef enum {
  THEMIS_HORN = CAN_ID(DEVICE_THEMIS, 0),
} ThemisMessageID;
