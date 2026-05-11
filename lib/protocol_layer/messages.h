#ifndef MESSAGES_H
#define MESSAGES_H


//======================================================//
// MESSAGE TYPES (REGISTERS)                            //
//======================================================//

#include <stdint.h>
#include <stdint.h>

#pragma once

/*
============================================================
 Communication Register Map
============================================================

 Register Layout:
 0x00 - 0x0F : System / Communication
 0x10 - 0x1F : Motor Control
 0x20 - 0x2F : Battery Management
 0x30 - 0x4F : Vehicle Sensors
 0x50 - 0xFD : Reserved / Future Expansion
 0xFE        : Start Of Transmission
 0xFF        : Status Byte

============================================================
*/


// =========================================================
// System / Communication Messages
// =========================================================
#define MSG_RESERVED_00               0x00  // Reserved - do not use
#define MSG_ESTABLISH_COMMUNICATION   0x01  // Request communication handshake
#define MSG_CONFIRM_COMMUNICATION     0x02  // Confirm successful communication handshake
#define MSG_EMERGENCY_STOP            0x03  // Immediately enter safe state
#define MSG_SET_SAFE_STATE            0x04  // Transition vehicle into safe state
#define MSG_SW_UPDATE_ESP32           0x05  // ESP32 firmware update notification
#define MSG_SW_UPDATE_MEGA            0x06  // Arduino Mega firmware update notification
#define MSG_ERROR_MESSAGE             0x07  // Error report, payload contains error code
#define MSG_COMMUNICATION_ERROR       0x08  // Packet corruption or transmission error
#define MSG_REQUEST_ESP32_VERSION     0x09  // Request ESP32 software version
#define MSG_REQUEST_MEGA_VERSION      0x0A  // Request Arduino Mega software version
#define MSG_RESERVED_0B               0x0B
#define MSG_RESERVED_0C               0x0C
#define MSG_RESERVED_0D               0x0D
#define MSG_RESERVED_0E               0x0E
#define MSG_RESERVED_0F               0x0F


// =========================================================
// Motor Control Messages
// =========================================================
#define MSG_STOP_MOTORS               0x10  // Stop all motors immediately
#define MSG_SET_MOTOR_SPEED           0x11  //motor control comands. Payload:int16_t per motor Range: -1000 to 1000 above or bellow this is an error. 
#define MSG_REQUEST_MOTOR_1_CURRENT   0x12  // Request current draw of motor 1
#define MSG_REQUEST_MOTOR_2_CURRENT   0x13  // Request current draw of motor 2
#define MSG_STALL_CURRENT_LIMIT       0x14  // Motor exceeded stall current limit
#define MSG_RESERVED_15               0x15
#define MSG_RESERVED_16               0x16
#define MSG_RESERVED_17               0x17
#define MSG_RESERVED_18               0x18
#define MSG_RESERVED_19               0x19
#define MSG_RESERVED_1A               0x1A
#define MSG_RESERVED_1B               0x1B
#define MSG_RESERVED_1C               0x1C
#define MSG_RESERVED_1D               0x1D
#define MSG_RESERVED_1E               0x1E
#define MSG_RESERVED_1F               0x1F


// =========================================================
// Battery Management
// =========================================================
#define MSG_RESERVED_20               0x20
#define MSG_RESERVED_21               0x21
#define MSG_RESERVED_22               0x22
#define MSG_RESERVED_23               0x23
#define MSG_RESERVED_24               0x24
#define MSG_RESERVED_25               0x25
#define MSG_RESERVED_26               0x26
#define MSG_RESERVED_27               0x27
#define MSG_RESERVED_28               0x28
#define MSG_RESERVED_29               0x29
#define MSG_RESERVED_2A               0x2A
#define MSG_RESERVED_2B               0x2B
#define MSG_RESERVED_2C               0x2C
#define MSG_RESERVED_2D               0x2D
#define MSG_RESERVED_2E               0x2E
#define MSG_RESERVED_2F               0x2F


// =========================================================
// Vehicle Sensors Bank 1
// =========================================================
#define MSG_RESERVED_30               0x30
#define MSG_RESERVED_31               0x31
#define MSG_RESERVED_32               0x32
#define MSG_RESERVED_33               0x33
#define MSG_RESERVED_34               0x34
#define MSG_RESERVED_35               0x35
#define MSG_RESERVED_36               0x36
#define MSG_RESERVED_37               0x37
#define MSG_RESERVED_38               0x38
#define MSG_RESERVED_39               0x39
#define MSG_RESERVED_3A               0x3A
#define MSG_RESERVED_3B               0x3B
#define MSG_RESERVED_3C               0x3C
#define MSG_RESERVED_3D               0x3D
#define MSG_RESERVED_3E               0x3E
#define MSG_RESERVED_3F               0x3F


// =========================================================
// Vehicle Sensors Bank 2
// =========================================================
#define MSG_RESERVED_40               0x40
#define MSG_RESERVED_41               0x41
#define MSG_RESERVED_42               0x42
#define MSG_RESERVED_43               0x43
#define MSG_RESERVED_44               0x44
#define MSG_RESERVED_45               0x45
#define MSG_RESERVED_46               0x46
#define MSG_RESERVED_47               0x47
#define MSG_RESERVED_48               0x48
#define MSG_RESERVED_49               0x49
#define MSG_RESERVED_4A               0x4A
#define MSG_RESERVED_4B               0x4B
#define MSG_RESERVED_4C               0x4C
#define MSG_RESERVED_4D               0x4D
#define MSG_RESERVED_4E               0x4E
#define MSG_RESERVED_4F               0x4F


// =========================================================
// Protocol Markers
// =========================================================
#define MSG_START_BYTE                0xFF  // Start of transmission packet


#endif