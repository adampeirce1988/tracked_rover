    

    #include <stdint.h>

    #pragma once

    //NOTE: Reserved IDs may only be allocated via versioned protocol change log.

    //===========================================================
    //Communication Type Register Map
    //===========================================================
    // Register Layout:
    // 0x00 - 0x0F : System / Communication
    // 0x10 - 0x1F : Motor Control
    // 0x20 - 0x2F : Battery Management
    // 0x30 - 0x4F : Vehicle Sensors
    // 0x50 - 0xEF : Reserved / Future Expansion
    // 0xf0 - 0xFF : Test commands

    //==========================================================



    // =========================================================
    // System / Communication Messages
    // =========================================================
    constexpr uint8_t MSG_RESERVED_00 =                     0x00; // Reserved - do not use
    constexpr uint8_t MSG_ESTABLISH_COMMUNICATION =         0x01; // Request communication handshake
    constexpr uint8_t MSG_CONFIRM_COMMUNICATION =           0x02; // Confirm successful communication handshake
    constexpr uint8_t MSG_EMERGENCY_STOP =                  0x03; // Immediately enter safe state
    constexpr uint8_t MSG_SET_SAFE_STATE =                  0x04; // Transition vehicle into safe state
    constexpr uint8_t MSG_SW_UPDATE_ESP32 =                 0x05; // ESP32 firmware update notification
    constexpr uint8_t MSG_SW_UPDATE_MEGA =                  0x06; // Arduino Mega firmware update notification
    constexpr uint8_t MSG_COMMUNICATION_ERROR =             0x07; // Packet corruption or transmission error
    constexpr uint8_t MSG_REQUEST_ESP32_VERSION =           0x08; // Request ESP32 software version
    constexpr uint8_t MSG_REQUEST_MEGA_VERSION =            0x09; // Request Arduino Mega software version
    constexpr uint8_t MSG_RESERVED_0A =                     0x0A;
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
    constexpr uint8_t MSG_REQUEST_MOTOR_2_CURRENT =         0x13; // Request current draw of motor 2
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
    // Vehicle Sensors Bank 3
    // =========================================================
    constexpr uint8_t MSG_RESERVED_50  =                   0x50;
    constexpr uint8_t MSG_RESERVED_51  =                   0x51;
    constexpr uint8_t MSG_RESERVED_52  =                   0x52;
    constexpr uint8_t MSG_RESERVED_53  =                   0x53;
    constexpr uint8_t MSG_RESERVED_54  =                   0x54;
    constexpr uint8_t MSG_RESERVED_55  =                   0x55;
    constexpr uint8_t MSG_RESERVED_56  =                   0x56;
    constexpr uint8_t MSG_RESERVED_57  =                   0x57;
    constexpr uint8_t MSG_RESERVED_58  =                   0x58;
    constexpr uint8_t MSG_RESERVED_59  =                   0x59;
    constexpr uint8_t MSG_RESERVED_5A  =                   0x5A;
    constexpr uint8_t MSG_RESERVED_5B  =                   0x5B;
    constexpr uint8_t MSG_RESERVED_5C  =                   0x5C;
    constexpr uint8_t MSG_RESERVED_5D  =                   0x5D;
    constexpr uint8_t MSG_RESERVED_5E  =                   0x5E;
    constexpr uint8_t MSG_RESERVED_5F  =                   0x5F;

    // =========================================================
    // Protocol Markers
    // =========================================================

    // =========================================================
    // Error messsages 1
    // =========================================================
    constexpr uint8_t MSG_ERROR_RESERVED_D0 =              0xD0;
    constexpr uint8_t MSG_ERROR_RESERVED_D1 =              0xD1;
    constexpr uint8_t MSG_ERROR_RESERVED_D2 =              0xD2;
    constexpr uint8_t MSG_ERROR_RESERVED_D3 =              0xD3;
    constexpr uint8_t MSG_ERROR_RESERVED_D4 =              0xD4;
    constexpr uint8_t MSG_ERROR_RESERVED_D5 =              0xD5;
    constexpr uint8_t MSG_ERROR_RESERVED_D6 =              0xD6;
    constexpr uint8_t MSG_ERROR_RESERVED_D7 =              0xD7;
    constexpr uint8_t MSG_ERROR_RESERVED_D8 =              0xD8;
    constexpr uint8_t MSG_ERROR_RESERVED_D9 =              0xD9;
    constexpr uint8_t MSG_ERROR_RESERVED_DA =              0xDA;
    constexpr uint8_t MSG_ERROR_RESERVED_DB =              0xDB;
    constexpr uint8_t MSG_ERROR_RESERVED_DC =              0xDC;
    constexpr uint8_t MSG_ERROR_RESERVED_DD =              0xDD;
    constexpr uint8_t MSG_ERROR_RESERVED_DE =              0xDE;
    constexpr uint8_t MSG_ERROR_RESERVED_DF =              0xDF;

    // =========================================================
    // Error messsages 2
    // =========================================================
    constexpr uint8_t MSG_ERROR_RESERVED_E0 =              0xE0;
    constexpr uint8_t MSG_ERROR_RESERVED_E1 =              0xE1;
    constexpr uint8_t MSG_ERROR_RESERVED_E2 =              0xE2;
    constexpr uint8_t MSG_ERROR_RESERVED_E3 =              0xE3;
    constexpr uint8_t MSG_ERROR_RESERVED_E4 =              0xE4;
    constexpr uint8_t MSG_ERROR_RESERVED_E5 =              0xE5;
    constexpr uint8_t MSG_ERROR_RESERVED_E6 =              0xE6;
    constexpr uint8_t MSG_ERROR_RESERVED_E7 =              0xE7;
    constexpr uint8_t MSG_ERROR_RESERVED_E8 =              0xE8;
    constexpr uint8_t MSG_ERROR_RESERVED_E9 =              0xE9;
    constexpr uint8_t MSG_ERROR_RESERVED_EA =              0xEA;
    constexpr uint8_t MSG_ERROR_RESERVED_EB =              0xEB;
    constexpr uint8_t MSG_ERROR_RESERVED_EC =              0xEC;
    constexpr uint8_t MSG_ERROR_RESERVED_ED =              0xED;
    constexpr uint8_t MSG_ERROR_RESERVED_EE =              0xEE;
    constexpr uint8_t MSG_ERROR_RESERVED_EF =              0xEF;

    // =========================================================
    // Protocol test variables 
    // =========================================================
    constexpr uint8_t MSG_TEST_INVALID =                   0xF0; // test call will always be invalid
    constexpr uint8_t MSG_TEST_INHIBITED =                 0xF1; // test call will valid but inhibited 
    constexpr uint8_t MSG_TEST_RESERVED =                  0xF2; // test call will always be reserved
    constexpr uint8_t MSG_TEST_VALID =                     0xF3; // test call will alays be valid
    constexpr uint8_t MSG_RESERVED_F4 =                    0xF4;
    constexpr uint8_t MSG_RESERVED_F5 =                    0xF5;
    constexpr uint8_t MSG_RESERVED_F6  =                   0xF6;
    constexpr uint8_t MSG_RESERVED_F7 =                    0xF7;
    constexpr uint8_t MSG_RESERVED_F8 =                    0xF8;
    constexpr uint8_t MSG_RESERVED_F9 =                    0xF9;
    constexpr uint8_t MSG_RESERVED_FA =                    0xFA;
    constexpr uint8_t MSG_RESERVED_FB =                    0xFB;
    constexpr uint8_t MSG_RESERVED_FC =                    0xFC;
    constexpr uint8_t MSG_RESERVED_FD =                    0xFD;
    constexpr uint8_t MSG_RESERVED_FE =                    0xFE;
    constexpr uint8_t MSG_RESERVED_FF =                    0xFF;
