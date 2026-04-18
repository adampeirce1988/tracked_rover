#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

// Ddefault transport type 
//#define DEFAULT_TRANSPORT &uart_io
#define DEFAULT_TRANSPORT &fifo_io  // used for simualting a link this will delive the tx to the rx port

// define the target platform here
#define TARGET_PLATFORM ESP32
//#define TARGET_PLATFORM MEGA2650

#if TARGET_PLATFORM == ESP32
    #define DEBUG_PORT_RX_PIN      16
    #define DEBUG_PORT_TX_PIN      17
    #define MSG_FORMAT             SERIAL_8N1

    #define DEBUG_PORT             Serial
    #define COMS_PORT              Serial1   

    #define DEBUG_PORT_BAUD        115200
    #define COMS_PORT_BAUD         115200

#elif TARGET_PLATFORM == MEGA2650
        #define DEBUG_PORT        Serial
        #define COMS_PORT         Serial1   

    #define DEBUG_PORT_BAUD        115200
    #define COMS_PORT_BAUD         115200
#else
    #error "TARGET_PLATFORM not defined or not supported. Please define TARGET_PLATFORM as either 'ESP32' or 'MEGA'."
#endif  

#define VERSION_ESP32    "1.10.0"
#define VERSION_MEGA     "1.10.0"

#endif // GLOBAL_CONFIG_H