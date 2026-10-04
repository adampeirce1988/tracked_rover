#ifndef UPDATE_INTERNAL_H
#define UPDATE_INTERNAL_H

#include <stddef.h>
#include "esp_partition.h"


//=============================================================================*
// OTA Internal Functions
//=============================================================================*

bool check_esp_ota_update_file_size(const esp_partition_t* partition, size_t firmware_size);
const esp_partition_t* get_esp_ota_update_partition();

#endif 