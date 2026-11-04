#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>

//=============================================================================*
// Server API Interface
//=============================================================================*

void server_init();
void server_run(); 

void web_socket_broadcast_text(const char* message); 


#endif 