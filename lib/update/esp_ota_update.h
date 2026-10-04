#ifndef ESP_OTA_UPDATE_H
#define ESP_OTA_UPDATE_H

//=============================================================================*
// ESP OTA Functions
//=============================================================================*

bool esp_update_begin(size_t firmware_size);
bool esp_update_write(const uint8_t* data, size_t length); 
bool esp_update_finalise(); 
bool esp_update_abort(); 
bool esp_update_set_boot_partition();
void esp_update_reboot(); 

//=============================================================================*
// ESP OTA Sub Functions
//=============================================================================*

bool check_esp_ota_update_file_size(const esp_partition_t* partition, size_t firmware_size);
const esp_partition_t* get_esp_ota_update_partition();


#endif