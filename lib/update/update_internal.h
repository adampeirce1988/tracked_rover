#ifndef UPDATES_INTERNAL_H
#define UPDATES_INTERNAL_H

#include <stddef.h>
#include "esp_partition.h"

//=============================================================================*
// OTA internal functions
//=============================================================================*

bool check_ota_update_file_size(const esp_partition_t* partition, size_t firmware_size);
const esp_partition_t* get_ota_update_partition();

bool ota_begin(size_t firmware_size);
bool ota_write(uint8_t data, size_t length); 
bool ota_finalise(); 
bool ota_set_boot_partition();
void ota_reboot(); 

#endif 