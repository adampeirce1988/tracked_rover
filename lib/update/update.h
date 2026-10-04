#ifndef UPDATE_H
#define UPDATE_H

#include <stdint.h>
#include <stddef.h>

//=============================================================================*
// Update Type
//=============================================================================*

enum class UPDATE_TYPE : uint8_t{
    NONE,
    FIRMWARE, 
    FILESYSTEM 
};

//=============================================================================*
// Update Diagnostic Functions 
//=============================================================================*
 
void esp_ota_print_diagnostics();

//=============================================================================*
// Update Status
//=============================================================================*

bool update_in_progress(); 
UPDATE_TYPE get_update_type();

//=============================================================================*
// Update Control
//=============================================================================*

bool update_begin(UPDATE_TYPE type, size_t update_size);
bool update_write(const uint8_t* data, size_t length);
bool update_finalise(); 
bool update_abort(); 

#endif 