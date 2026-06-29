#ifndef PERMISSIONS_H 
#define PERMISSIONS_H

#include <stdint.h>
#include "messages.h"

// this must match the system states
enum STATE_FLAG : uint8_t{
  SAFE_STATE_BIT    = (1<< 0),    // used only for OTA updating micro controlers.  
  IDLE_BIT          = (1<< 1), 
  MANUAL_BIT        = (1<< 2),
  AUTONOMOUS_BIT    = (1<< 3), 
  DIAGNOSTICS_BIT   = (1<< 4), 
  FAIL_SAFE_BIT     = (1<< 5)     // allow communication recovery messages only & block all control and motor commands
};

struct PERMISSION_STRUCT{
    uint8_t msg; 
    uint8_t bit_mask;
};


// all message that are defined must have rules allocated to allow them to b run
// NOTE: commands that move physical part must not be alocated STATE_FLAG::FAIL_SAFE_BIT. 
// NOT FOLLOWING THE ABOVE NOTE MAT CAUSE RISK OF DAMMAGE OR PERSONAL INJURY!
constexpr PERMISSION_STRUCT rules[] =
{   {MSG_ESTABLISH_COMMUNICATION, STATE_FLAG::SAFE_STATE_BIT | STATE_FLAG::FAIL_SAFE_BIT },
    {MSG_SET_MOTOR_SPEED, STATE_FLAG::MANUAL_BIT | STATE_FLAG::AUTONOMOUS_BIT | STATE_FLAG::IDLE_BIT },
    {MSG_TEST_INVALID, STATE_FLAG::DIAGNOSTICS_BIT}, 
    {MSG_TEST_VALID, STATE_FLAG::DIAGNOSTICS_BIT},
    {MSG_TEST_RESERVED, STATE_FLAG::DIAGNOSTICS_BIT}, 
    {MSG_TEST_INHIBITED, STATE_FLAG::DIAGNOSTICS_BIT}
};

enum class PERMISSION_RETURN_CODE{
    MSG_VALID_ALLOWED,
    MSG_VALID_INHIBITED,
    MSG_INVALID 
};

// function prototypes
PERMISSION_RETURN_CODE is_message_allowed_in_current_state(uint8_t type);

#endif