#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "protocol_types.h"

/*=============================================================================*
 * Protocol API
 *=============================================================================*/

PROTOCOL_RX_RETURN_CODE rx_message_task_dispatcher();


/*=============================================================================*
 * Communication Functions
 *=============================================================================*/

void establish_coms();
void confirm_coms();

#endif