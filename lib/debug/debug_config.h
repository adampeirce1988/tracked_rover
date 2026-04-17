#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H 


// define the files that you would like to debug here
  #define DEBUG_FILE_MASK   (DBG_TRANSPORT | DBG_DEBUG| DBG_ESP_MAIN | DBG_MEGA_MAIN | DBG_ESP_OTA | DBG_TRANSPORT_UART | DBG_TRANSPORT_FIFO) // use with arduino uno only

// define the levels of debuging required here
  //#define DEBUG_LEVEL_MASK  (DEBUG_MSG | DEBUG_INFO | DEBUG_ERROR | DEBUG_DEBUG)
  #define DEBUG_LEVEL_MASK (DEBUG_NONE | DEBUG_MSG | DEBUG_INFO | DEBUG_ERROR | DEBUG_DEBUG)  // use with arduino uno only


// defines which messages should be printed in full form DEBUG_PRINT_DATA_FRAME()
  #define DEBUG_FRAME_MASK (TX_FRAME | TX_PACK_MSG)

//------------ file bit mask ------------//
#define DBG_TRANSPORT            (1UL << 0)
#define DBG_PROTOCOL             (1UL << 1)
#define DBG_ESP_MAIN             (1UL << 2)
#define DBG_MEGA_MAIN            (1UL << 3)
#define DBG_ESP_OTA              (1UL << 4)
#define DBG_DEBUG                (1UL << 5)
#define DBG_TRANSPORT_UART       (1UL << 6)
#define DBG_TRANSPORT_FIFO       (1UL << 7)
// shift to 16bit mask for expansion

//------------ debug level bitmask------------ //

#define DEBUG_NONE    (1UL << 0)
#define DEBUG_MSG     (1UL << 1)
#define DEBUG_STREAM  (1UL << 2)
#define DEBUG_ERROR   (1UL << 3)
#define DEBUG_WARN    (1UL << 4)
#define DEBUG_INFO    (1UL << 5)
#define DEBUG_DEBUG   (1UL << 6)

//------------ debug print frame bitmask------------ //
#define TX_FRAME        (1UL << 0)
#define RX_FRAME        (1UL << 1)
#define ACK_FRAME       (1UL << 2)
#define TX_PACK_MSG     (1UL << 3)
#define TX_ACK_MSG      (1UL << 4)

#endif // DEBUG_CONFIG_H