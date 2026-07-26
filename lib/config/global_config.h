#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <stdint.h>


// define the target platform here
#define TARGET_PLATFORM ESP32
//#define TARGET_PLATFORM MEGA2650

//=================== main config ====================
constexpr uint32_t DIAGNOSTIC_WT_TIMEOUT_MS =   5000;                   // max diagnostic test run time before a timeout error
constexpr uint32_t SYSTEM_NOT_ALIVE_TIMEOUT =   2000;                   // start with 500ms (tune value once tested)
constexpr uint32_t COMMUNICATION_IDLE_WARN =     1000;
constexpr uint32_t COMMUNICATION_IDLE_TIMEOUT =  2000;
//===================================================


//================= transport config =================

// Ddefault transport type 
//#define DEFAULT_TRANSPORT &uart_io
#define DEFAULT_TRANSPORT          &uart_io              // used for simualting a link this will delive the tx to the rx port
#define DEFAULT_TRANSPORT_STR      uart_io               // naming convention used for debug should match the default transport          


constexpr uint32_t MSG_WDT_TIMEOUT_US =        35000;    // message watchdog time for bytes received updated after evey read in RX (us)  
constexpr uint32_t ACK_WDT_TIMEOUT_US =        65000;    // watch dog timer for ack (us) (slightly longer than one packet)
constexpr uint8_t MAX_PAYLOAD_LEN =            6;        // maximum payload of message frame 
constexpr uint8_t TX_MAX_RETRIES =             3;        // number of message send retries before reproting a failure
constexpr uint8_t PENDING_ACK_QUEUE_SIZE =     2;        // size for the pending ackq queue 

//=====================================================

#if TARGET_PLATFORM == ESP32

    constexpr char SW_VERSION[] =        "Software version 1.10.1";
    constexpr char HARDWARE_VERSION[] =  "ESP32 WROOM DEV board";

    // to be removed 
    #define RELEASE_NOTES          "-latest version of software contains a software uart simulator \n- *currently in development* this will allow a full slef test of the transport lay allowing testing of all failure types"
                             

    constexpr uint8_t DEBUG_PORT_RX_PIN =      16;
    constexpr uint8_t DEBUG_PORT_TX_PIN =      17;
    constexpr uint8_t MAX_SERIAL_BUFFER_SIZE = 128;

    #define MSG_FORMAT             SERIAL_8N1
    #define DEBUG_PORT             Serial 
    #define COMS_PORT              Serial1   

    constexpr uint32_t DEBUG_PORT_BAUD =        115200;
    constexpr uint32_t COMS_PORT_BAUD =         115200;

#elif TARGET_PLATFORM == MEGA2650

    #define SW_VERSION             "Software version 1.10.1\n"
    #define HARDWARE_VERSION       "ARDUINO MEGA 2650\n"
    #define RELEASE_NOTES          "-latest version of software contains a software uart simulator \n- *currently in development* this will allow a full slef test of the transport lay allowing testing of all failure types\n"
                             
    #define MAX_SERIAL_BUFFER_SIZE 64
    

    #define DEBUG_PORT             Serial
    #define COMS_PORT              Serial1   

    #define DEBUG_PORT_BAUD        115200
    #define COMS_PORT_BAUD         115200
#else
    #error "TARGET_PLATFORM not defined or not supported. Please define TARGET_PLATFORM as either 'ESP32' or 'MEGA'."
#endif  

// constexpr char* SW_VERSION_ESP32 =   "2.00.0"
// #define VERSION_MEGA     "1.10.0"

#endif // GLOBAL_CONFIG_H