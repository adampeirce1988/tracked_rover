#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>

/*=============================================================================*
 * Vehicle State
 *=============================================================================*/

enum class VEHICLE_STATE : uint8_t{
  BOOTING,          // all setup code will run ib here and not in main. 
  SAFE_STATE,       // allow communication recovery messages only & block all control and motor commands
  IDLE,             // vehicle idle awaiting commands 
  MANUAL,           // vehile under opporators control 
  AUTONOMOUS,       // vehicle oppeerates autonomusly (this will be the last to impliment)
  DIAGNOSTICS,      // Run on boad self tests and live data. 
  UPDATE,           // carry out an OTA update of the ESP 
  FAIL_SAFE,        // fault detected the vehicle will be stoped (may later only opperate in derate depending on the issue)
  SHUTTING_DOWN     // used to shut down the esp (all files that are open on LittleFS will be closed and saved) 
};

#endif