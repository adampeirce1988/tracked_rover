#ifndef ESP_OTA_UPDATE_H
#define ESP_OTA_UPDATE_H

#include <stddef.h>
#include <stdint.h>

//=============================================================================*
// ESP OTA Functions
//=============================================================================*

bool esp_update_begin(size_t firmware_size);
bool esp_update_write(const uint8_t* data, size_t length); 
bool esp_update_finalise(); 
bool esp_update_abort(); 
bool esp_update_set_boot_partition();
void esp_update_reboot();    // remove later if this is handled by the FSM. 


//=============================================================================*
// ESP OTA Diagnostics
//=============================================================================*

void esp_ota_print_diagnostics();

#endif