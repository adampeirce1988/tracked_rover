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
constexpr uint8_t MSG_RESERVED_00 =                     0x00;  // Reserved - do not use
constexpr uint8_t MSG_ESTABLISH_COMMUNICATION =         0x01; // Request communication handshake
constexpr uint8_t MSG_CONFIRM_COMMUNICATION =           0x02;  // Confirm successful communication handshake
constexpr uint8_t MSG_EMERGENCY_STOP =                  0x03;  // Immediately enter safe state
constexpr uint8_t MSG_SET_SAFE_STATE =                  0x04;  // Transition vehicle into safe state
constexpr uint8_t MSG_SW_UPDATE_ESP32 =                 0x05; // ESP32 firmware update notification
constexpr uint8_t MSG_SW_UPDATE_MEGA =                  0x06;  // Arduino Mega firmware update notification
constexpr uint8_t MSG_ERROR_MESSAGE =                   0x07;  // Error report, payload contains error code
constexpr uint8_t MSG_COMMUNICATION_ERROR =             0x08;  // Packet corruption or transmission error
constexpr uint8_t MSG_REQUEST_ESP32_VERSION =           0x09;  // Request ESP32 software version
constexpr uint8_t MSG_REQUEST_MEGA_VERSION =            0x0A; // Request Arduino Mega software version
constexpr uint8_t MSG_RESERVED_0B =                     0x0B;
constexpr uint8_t MSG_RESERVED_0C =                     0x0C;
constexpr uint8_t MSG_RESERVED_0D =                     0x0D;
constexpr uint8_t MSG_RESERVED_0E =                     0x0E;
constexpr uint8_t MSG_RESERVED_0F =                     0x0F;


// =========================================================
// Motor Control Messages
// =========================================================
constexpr uint8_t MSG_STOP_MOTORS =                     0x10; // Stop all motors immediately
constexpr uint8_t MSG_SET_MOTOR_SPEED =                 0x11; //motor control comands. Payload:int16_t per motor Range: -1000 to 1000 above or bellow this is an error. 
constexpr uint8_t MSG_REQUEST_MOTOR_1_CURRENT =         0x12; // Request current draw of motor 1
constexpr uint8_t MSG_REQUEST_MOTOR_2_CURRENT =         0x13;  // Request current draw of motor 2
constexpr uint8_t MSG_STALL_CURRENT_LIMIT =             0x14; // Motor exceeded stall current limit
constexpr uint8_t MSG_RESERVED_15 =                     0x15;
constexpr uint8_t MSG_RESERVED_16 =                     0x16;
constexpr uint8_t MSG_RESERVED_17 =                     0x17;
constexpr uint8_t MSG_RESERVED_18 =                     0x18;
constexpr uint8_t MSG_RESERVED_19 =                     0x19;
constexpr uint8_t MSG_RESERVED_1A =                     0x1A;
constexpr uint8_t MSG_RESERVED_1B =                     0x1B;
constexpr uint8_t MSG_RESERVED_1C =                     0x1C;
constexpr uint8_t MSG_RESERVED_1D =                     0x1D;
constexpr uint8_t MSG_RESERVED_1E =                     0x1E;
constexpr uint8_t MSG_RESERVED_1F =                     0x1F;


// =========================================================
// Battery Management
// =========================================================
constexpr uint8_t MSG_RESERVED_20 =                     0x20;
constexpr uint8_t MSG_RESERVED_21 =                     0x21;
constexpr uint8_t MSG_RESERVED_22 =                     0x22;
constexpr uint8_t MSG_RESERVED_23 =                     0x23;
constexpr uint8_t MSG_RESERVED_24 =                     0x24;
constexpr uint8_t MSG_RESERVED_25 =                     0x25;
constexpr uint8_t MSG_RESERVED_26 =                     0x26;
constexpr uint8_t MSG_RESERVED_27 =                     0x27;
constexpr uint8_t MSG_RESERVED_28 =                     0x28;
constexpr uint8_t MSG_RESERVED_29 =                     0x29;
constexpr uint8_t MSG_RESERVED_2A =                     0x2A;
constexpr uint8_t MSG_RESERVED_2B =                     0x2B;
constexpr uint8_t MSG_RESERVED_2C =                     0x2C;
constexpr uint8_t MSG_RESERVED_2D =                     0x2D;
constexpr uint8_t MSG_RESERVED_2E =                     0x2E;
constexpr uint8_t MSG_RESERVED_2F =                     0x2F;


// =========================================================
// Vehicle Sensors Bank 1
// =========================================================
constexpr uint8_t MSG_RESERVED_30 =                    0x30;
constexpr uint8_t MSG_RESERVED_31 =                    0x31;
constexpr uint8_t MSG_RESERVED_32 =                    0x32;
constexpr uint8_t MSG_RESERVED_33 =                    0x33;
constexpr uint8_t MSG_RESERVED_34 =                    0x34;
constexpr uint8_t MSG_RESERVED_35 =                    0x35;
constexpr uint8_t MSG_RESERVED_36 =                    0x36;
constexpr uint8_t MSG_RESERVED_37 =                    0x37;
constexpr uint8_t MSG_RESERVED_38 =                    0x38;
constexpr uint8_t MSG_RESERVED_39 =                    0x39;
constexpr uint8_t MSG_RESERVED_3A =                    0x3A;
constexpr uint8_t MSG_RESERVED_3B =                    0x3B;
constexpr uint8_t MSG_RESERVED_3C =                    0x3C;
constexpr uint8_t MSG_RESERVED_3D =                    0x3D;
constexpr uint8_t MSG_RESERVED_3E =                    0x3E;
constexpr uint8_t MSG_RESERVED_3F =                    0x3F;


// =========================================================
// Vehicle Sensors Bank 2
// =========================================================
constexpr uint8_t MSG_RESERVED_40  =                   0x40;
constexpr uint8_t MSG_RESERVED_41  =                   0x41;
constexpr uint8_t MSG_RESERVED_42  =                   0x42;
constexpr uint8_t MSG_RESERVED_43  =                   0x43;
constexpr uint8_t MSG_RESERVED_44  =                   0x44;
constexpr uint8_t MSG_RESERVED_45  =                   0x45;
constexpr uint8_t MSG_RESERVED_46  =                   0x46;
constexpr uint8_t MSG_RESERVED_47  =                   0x47;
constexpr uint8_t MSG_RESERVED_48  =                   0x48;
constexpr uint8_t MSG_RESERVED_49  =                   0x49;
constexpr uint8_t MSG_RESERVED_4A  =                   0x4A;
constexpr uint8_t MSG_RESERVED_4B  =                   0x4B;
constexpr uint8_t MSG_RESERVED_4C  =                   0x4C;
constexpr uint8_t MSG_RESERVED_4D  =                   0x4D;
constexpr uint8_t MSG_RESERVED_4E  =                   0x4E;
constexpr uint8_t MSG_RESERVED_4F  =                   0x4F;

// =========================================================
// Protocol Markers
// =========================================================

// =========================================================
// Protocol test variables 
// =========================================================
constexpr uint8_t MSG_TEST_INVALID =                   0xf0; // test call will always be invalid
constexpr uint8_t MSG_TEST_INHIBITED =                 0xf1; // test call will valid but inhibited 
constexpr uint8_t MSG_TEST_RESERVED =                  0xf2; // test call will always reserved
constexpr uint8_t MSG_TEST_VALID =                     0xf3;  // test call willvalid
constexpr uint8_t MSG_RESERVED_F4 =                    0xf4;
constexpr uint8_t MSG_RESERVED_F5 =                    0xf5;
constexpr uint8_t MSG_RESERVED_F6  =                   0xf6;
constexpr uint8_t MSG_RESERVED_F7 =                    0xf7;
constexpr uint8_t MSG_RESERVED_F8 =                    0xf8;
constexpr uint8_t MSG_RESERVED_F9 =                    0xf9;
constexpr uint8_t MSG_RESERVED_FA =                    0xfa;
constexpr uint8_t MSG_RESERVED_FB =                    0xfb;
constexpr uint8_t MSG_RESERVED_FC =                    0xfc;
constexpr uint8_t MSG_RESERVED_FD =                    0xfd;
constexpr uint8_t MSG_RESERVED_FE =                    0xfe;
constexpr uint8_t MSG_START_BYTE =                     0xff; // Start of transmission packet **RESERVED**

#endif