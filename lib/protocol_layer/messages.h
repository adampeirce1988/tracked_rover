#ifndef MESSAGES_H
#define MESSAGES_H


//======================================================//
// MESSAGE TYPES (REGISTERS)                            //
//======================================================//

#define REG_RESERVED_00         0x00  // do not use
#define ESTABLISH_COMUNICATION  0x01  // establish communication  
#define PAUSE_COMUNICAITON      0x08  // close comuication
#define EMERGANCY_STOP          0x03  // emergancy stop into safe state
#define SET_SAFE_STATE          0x04  // set the vehicle in safe state 
#define SW_UPDATE_ESP32         0x05  // notify that a software update of the ESP 
#define SW_UPDATE_MEGA          0x06  // notify that a software update of the MEGA 
#define ERROR_MESSAGE           0x07  // report an error data contains error code 
#define COMMUNICATION_ERROR     0x08  // report tansmission error
#define REG_RESERVED_09         0x09  // send ESP23 software version
#define REG_RESERVED_0A         0x0A  // Request arduino software version
#define REG_RESERVED_0B         0x0B
#define REG_RESERVED_0C         0x0C
#define REG_RESERVED_0D         0x0D
#define REG_RESERVED_0E         0x0E
#define REG_RESERVED_0F         0x0F 

//control messages
#define STOP_MOTORS             0x10 //stop motors 
#define SET_MOTOR_SPEED         0x11 //set motor speed 16bit per mortor (-32,768 - 32767). 
#define REQUEST_MOTOR_1_CURRNT  0x12 // request the current draw of morot 1 
#define REQUEST_MOTOR_2_CURRNT  0x13 // request the current draw of morot 2
#define STALL_CURRENT_LIMIT     0x14 // motor reached stall current limit
#define REG_RESERVED_15         0x15
#define REG_RESERVED_16         0x16
#define REG_RESERVED_17         0x17
#define REG_RESERVED_18         0x18
#define REG_RESERVED_19         0x19
#define REG_RESERVED_1A         0x1A
#define REG_RESERVED_1B         0x1B
#define REG_RESERVED_1C         0x1C
#define REG_RESERVED_1D         0x1D
#define REG_RESERVED_1E         0x1E
#define REG_RESERVED_1F         0x1F

// Battery managment
#define REG_RESERVED_20 0x20
#define REG_RESERVED_21 0x21
#define REG_RESERVED_22 0x22
#define REG_RESERVED_23 0x23
#define REG_RESERVED_24 0x24
#define REG_RESERVED_25 0x25
#define REG_RESERVED_26 0x26
#define REG_RESERVED_27 0x27
#define REG_RESERVED_28 0x28
#define REG_RESERVED_29 0x29
#define REG_RESERVED_2A 0x2A
#define REG_RESERVED_2B 0x2B
#define REG_RESERVED_2C 0x2C
#define REG_RESERVED_2D 0x2D
#define REG_RESERVED_2E 0x2E
#define REG_RESERVED_2F 0x2F

//vehicle sensors 1
#define REG_RESERVED_30 0x30
#define REG_RESERVED_31 0x31
#define REG_RESERVED_32 0x32
#define REG_RESERVED_33 0x33
#define REG_RESERVED_34 0x34
#define REG_RESERVED_35 0x35
#define REG_RESERVED_36 0x36
#define REG_RESERVED_37 0x37
#define REG_RESERVED_38 0x38
#define REG_RESERVED_39 0x39
#define REG_RESERVED_3A 0x3A
#define REG_RESERVED_3B 0x3B
#define REG_RESERVED_3C 0x3C
#define REG_RESERVED_3D 0x3D
#define REG_RESERVED_3E 0x3E
#define REG_RESERVED_3F 0x3F

//vehicle sensors 2 
#define REG_RESERVED_40 0x40
#define REG_RESERVED_41 0x41
#define REG_RESERVED_42 0x42
#define REG_RESERVED_43 0x43
#define REG_RESERVED_44 0x44
#define REG_RESERVED_45 0x45
#define REG_RESERVED_46 0x46
#define REG_RESERVED_47 0x47
#define REG_RESERVED_48 0x48
#define REG_RESERVED_49 0x49
#define REG_RESERVED_4A 0x4A
#define REG_RESERVED_4B 0x4B
#define REG_RESERVED_4C 0x4C
#define REG_RESERVED_4D 0x4D
#define REG_RESERVED_4E 0x4E
#define REG_RESERVED_4F 0x4F

#define REG_RESERVED_50 0x50
#define REG_RESERVED_51 0x51
#define REG_RESERVED_52 0x52
#define REG_RESERVED_53 0x53
#define REG_RESERVED_54 0x54
#define REG_RESERVED_55 0x55
#define REG_RESERVED_56 0x56
#define REG_RESERVED_57 0x57
#define REG_RESERVED_58 0x58
#define REG_RESERVED_59 0x59
#define REG_RESERVED_5A 0x5A
#define REG_RESERVED_5B 0x5B
#define REG_RESERVED_5C 0x5C
#define REG_RESERVED_5D 0x5D
#define REG_RESERVED_5E 0x5E
#define REG_RESERVED_5F 0x5F

#define REG_RESERVED_60 0x60
#define REG_RESERVED_61 0x61
#define REG_RESERVED_62 0x62
#define REG_RESERVED_63 0x63
#define REG_RESERVED_64 0x64
#define REG_RESERVED_65 0x65
#define REG_RESERVED_66 0x66
#define REG_RESERVED_67 0x67
#define REG_RESERVED_68 0x68
#define REG_RESERVED_69 0x69
#define REG_RESERVED_6A 0x6A
#define REG_RESERVED_6B 0x6B
#define REG_RESERVED_6C 0x6C
#define REG_RESERVED_6D 0x6D
#define REG_RESERVED_6E 0x6E
#define REG_RESERVED_6F 0x6F

#define REG_RESERVED_70 0x70
#define REG_RESERVED_71 0x71
#define REG_RESERVED_72 0x72
#define REG_RESERVED_73 0x73
#define REG_RESERVED_74 0x74
#define REG_RESERVED_75 0x75
#define REG_RESERVED_76 0x76
#define REG_RESERVED_77 0x77
#define REG_RESERVED_78 0x78
#define REG_RESERVED_79 0x79
#define REG_RESERVED_7A 0x7A
#define REG_RESERVED_7B 0x7B
#define REG_RESERVED_7C 0x7C
#define REG_RESERVED_7D 0x7D
#define REG_RESERVED_7E 0x7E
#define REG_RESERVED_7F 0x7F

#define REG_RESERVED_80 0x80
#define REG_RESERVED_81 0x81
#define REG_RESERVED_82 0x82
#define REG_RESERVED_83 0x83
#define REG_RESERVED_84 0x84
#define REG_RESERVED_85 0x85
#define REG_RESERVED_86 0x86
#define REG_RESERVED_87 0x87
#define REG_RESERVED_88 0x88
#define REG_RESERVED_89 0x89
#define REG_RESERVED_8A 0x8A
#define REG_RESERVED_8B 0x8B
#define REG_RESERVED_8C 0x8C
#define REG_RESERVED_8D 0x8D
#define REG_RESERVED_8E 0x8E
#define REG_RESERVED_8F 0x8F

#define REG_RESERVED_90 0x90
#define REG_RESERVED_91 0x91
#define REG_RESERVED_92 0x92
#define REG_RESERVED_93 0x93
#define REG_RESERVED_94 0x94
#define REG_RESERVED_95 0x95
#define REG_RESERVED_96 0x96
#define REG_RESERVED_97 0x97
#define REG_RESERVED_98 0x98
#define REG_RESERVED_99 0x99
#define REG_RESERVED_9A 0x9A
#define REG_RESERVED_9B 0x9B
#define REG_RESERVED_9C 0x9C
#define REG_RESERVED_9D 0x9D
#define REG_RESERVED_9E 0x9E
#define REG_RESERVED_9F 0x9F

#define REG_RESERVED_A0 0xA0
#define REG_RESERVED_A1 0xA1
#define REG_RESERVED_A2 0xA2
#define REG_RESERVED_A3 0xA3
#define REG_RESERVED_A4 0xA4
#define REG_RESERVED_A5 0xA5
#define REG_RESERVED_A6 0xA6
#define REG_RESERVED_A7 0xA7
#define REG_RESERVED_A8 0xA8
#define REG_RESERVED_A9 0xA9
#define REG_RESERVED_AA 0xAA
#define REG_RESERVED_AB 0xAB
#define REG_RESERVED_AC 0xAC
#define REG_RESERVED_AD 0xAD
#define REG_RESERVED_AE 0xAE
#define REG_RESERVED_AF 0xAF

#define REG_RESERVED_B0 0xB0
#define REG_RESERVED_B1 0xB1
#define REG_RESERVED_B2 0xB2
#define REG_RESERVED_B3 0xB3
#define REG_RESERVED_B4 0xB4
#define REG_RESERVED_B5 0xB5
#define REG_RESERVED_B6 0xB6
#define REG_RESERVED_B7 0xB7
#define REG_RESERVED_B8 0xB8
#define REG_RESERVED_B9 0xB9
#define REG_RESERVED_BA 0xBA
#define REG_RESERVED_BB 0xBB
#define REG_RESERVED_BC 0xBC
#define REG_RESERVED_BD 0xBD
#define REG_RESERVED_BE 0xBE
#define REG_RESERVED_BF 0xBF

#define REG_RESERVED_C0 0xC0
#define REG_RESERVED_C1 0xC1
#define REG_RESERVED_C2 0xC2
#define REG_RESERVED_C3 0xC3
#define REG_RESERVED_C4 0xC4
#define REG_RESERVED_C5 0xC5
#define REG_RESERVED_C6 0xC6
#define REG_RESERVED_C7 0xC7
#define REG_RESERVED_C8 0xC8
#define REG_RESERVED_C9 0xC9
#define REG_RESERVED_CA 0xCA
#define REG_RESERVED_CB 0xCB
#define REG_RESERVED_CC 0xCC
#define REG_RESERVED_CD 0xCD
#define REG_RESERVED_CE 0xCE
#define REG_RESERVED_CF 0xCF

#define REG_RESERVED_D0 0xD0
#define REG_RESERVED_D1 0xD1
#define REG_RESERVED_D2 0xD2
#define REG_RESERVED_D3 0xD3
#define REG_RESERVED_D4 0xD4
#define REG_RESERVED_D5 0xD5
#define REG_RESERVED_D6 0xD6
#define REG_RESERVED_D7 0xD7
#define REG_RESERVED_D8 0xD8
#define REG_RESERVED_D9 0xD9
#define REG_RESERVED_DA 0xDA
#define REG_RESERVED_DB 0xDB
#define REG_RESERVED_DC 0xDC
#define REG_RESERVED_DD 0xDD
#define REG_RESERVED_DE 0xDE
#define REG_RESERVED_DF 0xDF

#define REG_RESERVED_E0 0xE0
#define REG_RESERVED_E1 0xE1
#define REG_RESERVED_E2 0xE2
#define REG_RESERVED_E3 0xE3
#define REG_RESERVED_E4 0xE4
#define REG_RESERVED_E5 0xE5
#define REG_RESERVED_E6 0xE6
#define REG_RESERVED_E7 0xE7
#define REG_RESERVED_E8 0xE8
#define REG_RESERVED_E9 0xE9
#define REG_RESERVED_EA 0xEA
#define REG_RESERVED_EB 0xEB
#define REG_RESERVED_EC 0xEC
#define REG_RESERVED_ED 0xED
#define REG_RESERVED_EE 0xEE
#define REG_RESERVED_EF 0xEF

#define REG_RESERVED_F0 0xF0
#define REG_RESERVED_F1 0xF1
#define REG_RESERVED_F2 0xF2
#define REG_RESERVED_F3 0xF3
#define REG_RESERVED_F4 0xF4
#define REG_RESERVED_F5 0xF5
#define REG_RESERVED_F6 0xF6
#define REG_RESERVED_F7 0xF7
#define REG_RESERVED_F8 0xF8
#define REG_RESERVED_F9 0xF9
#define REG_RESERVED_FA 0xFA
#define REG_RESERVED_FB 0xFB
#define REG_RESERVED_FC 0xFC
#define REG_RESERVED_FD 0xFD  
#define START_BYTE            0xFE  // start of transmission 
#define END_BYTE              0xFF  // end of transmission (not used)





#endif 