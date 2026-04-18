
#ifndef DEBUG_H
#define DEBUG_H 

  #include <Arduino.h>
  #include "debug_config.h" 

  extern void debug_port_begin();

  #define DEBUG_PRINT_MSG(file, level, type, msg) \
    do {\
      if ((DEBUG_FILE_MASK & (file)) &&  (DEBUG_LEVEL_MASK & (level))) {\
        PRINT_RUNTIME(millis());\
        PRINT_FILE_NAME(file);\
        PRINT_LEVEL(level);\
        DEBUG_PORT.print("[");\
        DEBUG_PORT.print(type);\
        DEBUG_PORT.print("]");\
        DEBUG_PORT.println(msg);\
      }\
    }\
    while(0)

  #define DEBUG_PRINT_MSG_VAL(file, level, type, msg, val)\
    do {\
      if ((DEBUG_FILE_MASK & (file)) &&  (DEBUG_LEVEL_MASK & (level))) {\
        PRINT_RUNTIME(millis());\
        PRINT_FILE_NAME(file);\
        PRINT_LEVEL(level);\
        DEBUG_PORT.print("[");\
        DEBUG_PORT.print(type);\
        DEBUG_PORT.print("]");\
        DEBUG_PORT.print(msg);\
        DEBUG_PORT.println(val);\
      }\
    } while(0)

    #define DEBUG_PRINT_MSG_VAL_HEX(file, level, type, msg, val) \
    do {\
      if ((DEBUG_FILE_MASK & (file)) &&  (DEBUG_LEVEL_MASK & (level))) {\
        PRINT_RUNTIME(millis());\
        PRINT_FILE_NAME(file);\
        PRINT_LEVEL(level);\
        DEBUG_PORT.print("[");\
        DEBUG_PORT.print("][");\
        DEBUG_PORT.print(type);\
        DEBUG_PORT.print("]");\
        DEBUG_PORT.print(msg);\
        DEBUG_PORT.println(val, HEX);\
      }\
    } while(0)



  #define DEBUG_PRINT_DATA_FRAME(file, level, frame_type, start_byte, frame_name, frame)\
    do{\
      if((DEBUG_FILE_MASK & (file)) &&  (DEBUG_LEVEL_MASK & (level)) && (DEBUG_FRAME_MASK & (frame_type))) {\
        PRINT_RUNTIME(millis());\
        PRINT_FILE_NAME(file);\
        PRINT_LEVEL(level);\
        DEBUG_PORT.print(frame_name);\
        DEBUG_PORT.print("]");\
        DEBUG_PORT.print(start_byte, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame.TYPE, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame.ACK, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame.ID, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame.DLC, HEX);\
        DEBUG_PORT.print(", ");\
        for(uint8_t i = 0; i < frame.DLC; i ++){\
          DEBUG_PORT.print(frame.payload[i], HEX);\
          DEBUG_PORT.print(", ");\
        }\
        DEBUG_PORT.println(frame.CRC, HEX);\
      }\
    } while (0)

    #define DEBUG_PRINT_DATA_PTR_FRAME(file, level, frame_type, start_byte, frame_name, frame)\
    do{\
      if((DEBUG_FILE_MASK & (file)) &&  (DEBUG_LEVEL_MASK & (level)) && (DEBUG_FRAME_MASK & (frame_type))) {\
        PRINT_RUNTIME(millis());\
        PRINT_FILE_NAME(file);\
        PRINT_LEVEL(level);\
        DEBUG_PORT.print(frame_name);\
        DEBUG_PORT.print("]");\
        DEBUG_PORT.print(start_byte, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame->TYPE, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame->ACK, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame->ID, HEX);\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(frame->DLC, HEX);\
        DEBUG_PORT.print(", ");\
        for(uint8_t i = 0; i < frame->DLC; i ++){\
          DEBUG_PORT.print(frame->payload[i], HEX);\
          DEBUG_PORT.print(", ");\
        }\
        DEBUG_PORT.println(frame->CRC, HEX);\
      }\
    } while (0)
  

  #define DEBUG_STREAM_START(file, level, msg, byte)\
    do{\
      if((DEBUG_FILE_MASK & (file)) && (DEBUG_LEVEL_MASK & (level))) {\
        DEBUG_PORT.print("\n");\
        PRINT_RUNTIME(millis());\
        PRINT_FILE_NAME(file);\
        PRINT_LEVEL(level);\
        DEBUG_PORT.print("msg");\
        DEBUG_PORT.print(byte ,HEX);\
      }\
    } while(0)

  #define DEBUG_STREAM_DATA(file, level, byte)\
    do{\
      if((DEBUG_FILE_MASK & (file)) && (DEBUG_LEVEL_MASK & (level))) {\
        DEBUG_PORT.print(", ");\
        DEBUG_PORT.print(byte ,HEX);\
      }\
    } while(0)

  #define DEBUG_STREAM_END(file, level, byte)\
    do{\
      if((DEBUG_FILE_MASK & (file)) && (DEBUG_LEVEL_MASK & (level))) {\
        DEBUG_PORT.print(byte ,HEX);\
      }\
    } while(0)


  #define PRINT_RUNTIME(ms)\
    do {\
      uint32_t _t = ms;\
      uint32_t seconds = (_t) / 1000;\
      uint32_t millis_part = (_t) % 1000;\
      DEBUG_PORT.print("[");\
      if(seconds < 1000000) {DEBUG_PORT.print("0");}\
      if(seconds < 100000){  DEBUG_PORT.print("0");}\
      if(seconds < 10000){   DEBUG_PORT.print("0");}\
      if(seconds < 1000){    DEBUG_PORT.print("0");}\
      if(seconds < 100){     DEBUG_PORT.print("0");}\
      if(seconds < 10){      DEBUG_PORT.print("0");}\
      DEBUG_PORT.print(seconds);\
      DEBUG_PORT.print(".");\
      if(millis_part < 100){ DEBUG_PORT.print("0");}\
      if(millis_part < 10){  DEBUG_PORT.print("0");}\
      DEBUG_PORT.print(millis_part);\
      DEBUG_PORT.print("]");\
    } while(0)

  #define PRINT_FILE_NAME(file_name)\
    do{\
      if((file_name) & DBG_MEGA_MAIN)             {DEBUG_PORT.print("[MEGA]");}\
      else if ((file_name) & DBG_ESP_MAIN)        {DEBUG_PORT.print("[ESP]");}\
      else if ((file_name) & DBG_TRANSPORT)       {DEBUG_PORT.print("[TRAN]");}\
      else if ((file_name) & DBG_PROTOCOL)        {DEBUG_PORT.print("[PROT]");}\
      else if ((file_name) & DBG_ESP_OTA)         {DEBUG_PORT.print("[OTA]");}\
      else if ((file_name) & DBG_TRANSPORT_UART)  {DEBUG_PORT.print("[UART]");}\
      else{DEBUG_PORT.print("[----]");}\
    } while(0)

  #define PRINT_LEVEL(dbg_level)\
    do{\
      if((dbg_level) & DEBUG_ERROR)          {DEBUG_PORT.print("[ERROR]");}\
      else if((dbg_level) & DEBUG_WARN)      {DEBUG_PORT.print("[WARN]");}\
      else if((dbg_level) & DEBUG_INFO)      {DEBUG_PORT.print("[INFO]");}\
      else if((dbg_level) & DEBUG_DEBUG)     {DEBUG_PORT.print("[DEBUG]");}\
      else if((dbg_level) & DEBUG_MSG)       {DEBUG_PORT.print("[MSG]");}\
      else if ((dbg_level) & DEBUG_NONE)     {DEBUG_PORT.print("[NONE]");}\
      else{DEBUG_PORT.print("[----]");}\
    } while(0)


    #define DEBUG_PRINT_FIFO_VAR(var1, var2, var3, var4)\
    do{\
      DEBUG_PORT.print(" head: "); DEBUG_PORT.print(var1);\
      DEBUG_PORT.print(", tail: "); DEBUG_PORT.print(var2);\
      DEBUG_PORT.print(", tx_delay "); DEBUG_PORT.print(var3);\
      DEBUG_PORT.print(" ,last_rx_read; "); DEBUG_PORT.print(var4);\
      DEBUG_PORT.print (" , internal_avaliable: ");\
    } while(0)
#endif