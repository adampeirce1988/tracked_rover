#ifndef UPDATE_H
#define UPDATE_H

#include "debug.h"

#define DEUBG_FILE DBG_OTA


// diagnostic / debig calls 
void ota_print_update_partition();
void ota_print_patrition_data();

// move to internal calls 
void get_ota_update_partition();
bool check_ota_update_file_size(size_t firmware_size);
bool ota_begin(size_t firmware_size);

#endif 