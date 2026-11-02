#ifndef NETWORK_H
#define NETWORK_H


/*=============================================================================*
 * API calls 
*=============================================================================*/

void wifi_init();                      // initiates the wifi
void server_init();                    // initiates the server 
void run_server();                     // run the server (must be called evey loop)

int8_t wifi_signal_strength();        // returns the signal strenght in RSI

const char* wifi_current_mode();    // returns the current connetcion type (STA / AP)


#endif 


