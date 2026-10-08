
#include <Arduino.h>
#include "debug.h"
#include "update.h"
#include "esp_ota_update.h"

        
//=============================================================================*
// Debug 
//=============================================================================*

#define DEBUG_FILE DBG_ESP_OTA


//=============================================================================*
// OTA Variables
//=============================================================================*

static UPDATE_TYPE active_update_type = UPDATE_TYPE::NONE;    // Cache the current update type 


//=============================================================================*
// Update functions
//=============================================================================*

bool update_begin(UPDATE_TYPE type, size_t update_size){

    // guard against calling this function again once an update has started 
    if(active_update_type != UPDATE_TYPE::NONE){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update_begin() called during an active_update update");
        return false; 
    }

    if(type == UPDATE_TYPE::FIRMWARE){
        // Start the OTA firmware update.
        if(!esp_update_begin(update_size)){
            return false;
        }

        active_update_type = type; 
    }
    else if(type == UPDATE_TYPE::FILESYSTEM){
        // file system update goes here

        return false; 

    }
    else{
        // gaurd against invalid calls 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Invalid update type requested");
        return false; 
    }

    return true; 
 
}


bool update_write(const uint8_t* data, size_t length){

    // gaurd agains invalid data 
    if(data == nullptr){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update write received invalid data");
        return false; 
    }
    // gaurd agains 0 length
    if(length == 0){ 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update write received invalid length"); 
        return false; 
    } 


    // pass this chunk to the active update mechanism
    if(active_update_type == UPDATE_TYPE::FIRMWARE){
        return esp_update_write(data, length);
    }
    else if(active_update_type == UPDATE_TYPE::FILESYSTEM){
        // filesystem write goes here
        return false;
    }
    else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update write requested with no active update");
        return false;
    }
}


bool update_finalise(){
    // complete and validate the active update

    if(active_update_type == UPDATE_TYPE::FIRMWARE){
        
        if(!esp_update_finalise()){
            // error reported via ota_finalise()
            return false; 
        }

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "UDAT", "Update finalised");

        if(!esp_update_set_boot_partition()){
            // clear the current_update_type if partition change fails
            active_update_type = UPDATE_TYPE::NONE;

            return false; 
        }

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "UDAT", "Boot partition updated");

        active_update_type = UPDATE_TYPE::NONE; // reset the active update on succsess. 

        esp_ota_print_diagnostics();
        // to be decided - reboot should go here. 

        return true; 
    }
    else if(active_update_type == UPDATE_TYPE::FILESYSTEM){
        // Filesystem finalise code goes here. 
        return false; 

    }
    else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update finalise requested with no active update");
        return false;
    }

}

bool update_abort(){

    if(active_update_type == UPDATE_TYPE::FIRMWARE){

        if(!esp_update_abort()){
            return false; 
        }

        active_update_type = UPDATE_TYPE::NONE; 
        return true; 
    }
    else if(active_update_type == UPDATE_TYPE::FILESYSTEM){

        // Filesyststem update abort - gos here

        active_update_type = UPDATE_TYPE::NONE;
        return true; 
    }
    else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update abort requested with no active update");
        return false; 
    }
}


//=============================================================================*
// Update status
//=============================================================================*

bool update_in_progress(){

    if(active_update_type != UPDATE_TYPE::NONE){
        return true; 
    }

    return false; 
}

UPDATE_TYPE get_update_type(){

    return active_update_type; 
}


//=============================================================================*
// Update diagnosticd
//=============================================================================*

void firmware_update_diagnostics_readout(){

    esp_ota_print_diagnostics();

}

void filesystem_update_diagnostics_readout(){

    // file system diag goes here. 
}
