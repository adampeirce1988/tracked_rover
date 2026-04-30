#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H


// define the target platform here
#define TARGET_PLATFORM ESP32
//#define TARGET_PLATFORM MEGA2650

//================= transport config =================

// Ddefault transport type 
//#define DEFAULT_TRANSPORT &uart_io
#define DEFAULT_TRANSPORT          &uart_io              // used for simualting a link this will delive the tx to the rx port
#define DEFAULT_TRANSPORT_STR      uart_io               // naming convention used for debug should match the default transport          

//////////////////////////////////////
#define QUEUE_SIZE                 3
#define PENDING_ACK_QUEUE_SOZE     2
//////////////////////////////////////

#define START_BYTE                 0xFF                  // default start byte 0xFF
#define MSG_WDT_TIMEOUT_US         35000                 // message watchdog time for bytes received updated after evey read in RX (us)  
#define ACK_WDT_TIMEOUT_US         65000                 // watch dog timer for ack (us)
#define MAX_PAYLOAD_LEN            6                     // maximum payload of message frame 
#define PACKET_INCREMENT           1                     // used for debug only
#define TX_MAX_RETRIES             3                     // number of message send retries before reproting a failure


//=====================================================

#if TARGET_PLATFORM == ESP32

    #define SW_VERSION             "Software version 1.10.1"
    #define HARDWARE_VERSION       "ESP32 WROOM DEV board"
    #define RELEASE_NOTES          "-latest version of software contains a software uart simulator \n- *currently in development* this will allow a full slef test of the transport lay allowing testing of all failure types"
                             

    #define DEBUG_PORT_RX_PIN      16
    #define DEBUG_PORT_TX_PIN      17
    #define MSG_FORMAT             SERIAL_8N1
    #define MAX_SERIAL_BUFFER_SIZE 128

    #define DEBUG_PORT             Serial
    #define COMS_PORT              Serial1   

    #define DEBUG_PORT_BAUD        115200
    #define COMS_PORT_BAUD         115200

#elif TARGET_PLATFORM == MEGA2650

    #define SW_VERSION             "Software version 1.10.1\n"
    #define HARDWARE_VERSION       "ARDUINO MEGA 2650\n"
    #define RELEASE_NOTES          "-latest version of software contains a software uart simulator \n- *currently in development* this will allow a full slef test of the transport lay allowing testing of all failure types\n"
                             
    #define MAX_SERIAL_BUFFER SIZE 64
    

    #define DEBUG_PORT             Serial
    #define COMS_PORT              Serial1   

    #define DEBUG_PORT_BAUD        115200
    #define COMS_PORT_BAUD         115200
#else
    #error "TARGET_PLATFORM not defined or not supported. Please define TARGET_PLATFORM as either 'ESP32' or 'MEGA'."
#endif  

#define VERSION_ESP32    "1.10.0"
#define VERSION_MEGA     "1.10.0"

#endif // GLOBAL_CONFIG_H