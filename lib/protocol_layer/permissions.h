#ifndef PERMISSIONS_H 
#define PERMISSIONS_H

#include <stdint.h>
#include "messages.h"

// this must match the system states
enum STATE_MASK : uint8_t{
  SAFE_STATE_BIT    = (1<< 0),  
  IDLE_BIT          = (1<< 1), 
  MANUAL_BIT        = (1<< 2),
  AUTONOMOUS_BIT    = (1<< 3), 
  DIAGNOSTICS_BIT   = (1<< 4), 
  FAIL_SAFE_BIT     = (1<< 5) 
};

struct PERMISSION_STRUCT{
    uint8_t msg; 
    uint8_t bit_mask;
};

constexpr PERMISSION_STRUCT rules[] =
{    {MSG_ESTABLISH_COMMUNICATION, STATE_MASK::SAFE_STATE_BIT },
    {MSG_SET_MOTOR_SPEED, STATE_MASK::MANUAL_BIT | STATE_MASK::AUTONOMOUS_BIT | STATE_MASK::IDLE_BIT },
    {MSG_TEST_INVALID, STATE_MASK::DIAGNOSTICS_BIT}, 
    {MSG_TEST_VALID, STATE_MASK::DIAGNOSTICS_BIT},
    {MSG_TEST_RESERVED, STATE_MASK::DIAGNOSTICS_BIT}, 
    {MSG_TEST_INHIBITED, STATE_MASK::DIAGNOSTICS_BIT}
};

enum class PERMISSION_RETURN_CODE{
    MSG_VALID_ALLOWED,
    MSG_VALID_INHIBITED,
    MSG_INVALID 
};

// function prototypes
PERMISSION_RETURN_CODE message_permitted_in_current_state(uint8_t type);

#endif