#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>

/*=============================================================================*
 * Vehicle State
 *=============================================================================*/

enum class VEHICLE_STATE : uint8_t{
  BOOTING,          // Run all system setup code.
  SAFE_STATE,       // Allow communication recovery only; block control commands.
  IDLE,             // Vehicle idle and awaiting commands.
  MANUAL,           // vehicle under oporators control 
  AUTONOMOUS,       // Vehicle operates autonomously.
  DIAGNOSTICS,      // Run onboard self-tests and live diagnostics.
  UPDATE,           // Carry out an OTA update.
  FAIL_SAFE,        // Fault detected; vehicle is stopped and made safe.
  SHUTTING_DOWN     // Save and close files before entering low-power state.
};

/*=============================================================================*
 * Vehicle State Return Codes
 *=============================================================================*/

enum class VEHICLE_STATE_RETURN_CODE : uint8_t
{
    RUNNING,
    CHANGE_DENIED,
    CHANGE_APPROVED
};

#endif