
#include <Arduino.h>
#include "debug.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "update.h"
#include "update_internal.h"
        
#define DEBUG_FILE DBG_UPDATE


//=============================================================================*
// OTA varaiables
//=============================================================================*

static esp_ota_handle_t ota_handle = 0;                       // Cariable feteched by ota begin to carry out update. 
static const esp_partition_t* ota_partition = nullptr;        // Cache the partition pointer 
static UPDATE_TYPE active_update_type = UPDATE_TYPE::NONE;    // Cache the current update type 


//=============================================================================*
// Update Core Function
//=============================================================================*




//=============================================================================*
// Update Sub functions
//=============================================================================*

bool update_begin(UPDATE_TYPE type, size_t update_size){

    // gaurd against calling this founction agian once an update has started 
    if(active_update_type != UPDATE_TYPE::NONE){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update_begin() called during an active_update update");
        return false; 
    }

    if(type == UPDATE_TYPE::FIRMWARE){
        // Start the OTA firmware update.
        if(!ota_begin(update_size)){
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
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update requested while an update is in progress");
        active_update_type = UPDATE_TYPE::NONE; 
        return false; 
    }

    return true; 
 
}


bool update_write(const uint8_t* data, size_t length){

    // gaurd agains invalid data 
    if(data == nullptr){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update wirte receive invalid data");
        return false; 
    }
    // gaurd agains 0 length
    if(length == 0){ 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update write recived invalid length"); 
        return false; 
    } 


    // pass this chunk to the active update mechanism
    if(active_update_type == UPDATE_TYPE::FIRMWARE){
        return ota_write(data, length);
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
        
        if(!ota_finalise()){
            return false; 
        }

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update finalised");

        if(!ota_set_boot_partition()){

            // clear the current_update_type if partition change fails
            active_update_type = UPDATE_TYPE::NONE;
            return false; 
        }

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Boot partintion updated");

        active_update_type = UPDATE_TYPE::NONE; // reset the active update on sucsess. 
        return true; 
    }
    else if(active_update_type == UPDATE_TYPE::FILESYSTEM){
        // Filesystem finalise code gos here. 
        return false; 

    }
    else{
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Update finalise requested with no active update");
        return false;
    }

}

bool update_abort(){

    if(active_update_type == UPDATE_TYPE::FIRMWARE){

        if(!ota_abort()){
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
// OTA Update Sub Functions
//=============================================================================*

bool ota_begin(size_t firmware_size){

    // cache the partition being used for this OTA operation.
    ota_partition = get_ota_update_partition();

    // Check that an OTA partition is available.
    if(ota_partition == nullptr){
        return false;
    }

    // Check that the firmware will fit within the OTA partition.
    if(!check_ota_update_file_size(ota_partition, firmware_size)){
        return false;
    }

    // Start the OTA update on the selected partition.
    esp_err_t result = esp_ota_begin(ota_partition, firmware_size, &ota_handle);

    if(result != ESP_OK){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "OTA update failed to initiate");
        return false; 
    }
    
    return true; 

}


bool ota_write(const uint8_t* data, size_t length){

    esp_err_t result = esp_ota_write(ota_handle, data, length); 

    if(result != ESP_OK){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "OTA update failed to write to partition");
        return false; 
    }

    return true; 
}


bool ota_finalise(){

    esp_err_t result = esp_ota_end(ota_handle); 

    if(result != ESP_OK){ 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "OTA image failed to sucessfuly verrify");
        return false; 
    }

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "OTA", "OTA write finalised");

    ota_handle = 0; // rest the handle variable. 

    return true;

}

bool ota_abort(){ 
    esp_err_t result = esp_ota_abort(ota_handle); 

    if(result != ESP_OK){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "OTA update failed to abort");
        return false; 
    }

    ota_handle = 0; 
    ota_partition = nullptr; 

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "OTA", "OTA update aborted");

    return true; 
    
}

bool ota_set_boot_partition(){

    // Check that an OTA partition is available.
    if (ota_partition == nullptr){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "No OTA partition available to set as boot partition");
        return false; 
    }

    esp_err_t result = esp_ota_set_boot_partition(ota_partition);

    if(result != ESP_OK){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "Failed to set OTA partition as boot partition");
        return false;
    }
     
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "OTA", "OTA boot partition updated sucsessfully");
    return true; 

}

void ota_reboot(){

    // change to info once the the web interface reports the rebooting messae
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "Rebooting system");   
    
    // reboot the ESP
    ESP.restart();
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
// OTA Internal Functions
//=============================================================================*

const esp_partition_t* get_ota_update_partition(){

    return esp_ota_get_next_update_partition(nullptr); 

} 


bool check_ota_update_file_size(const esp_partition_t* partition, size_t firmware_size){

   // Check that an OTA partition is available. 
   if(partition == nullptr){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "No partition exsists to write update file");
        return false; 
   }
   
   if(firmware_size > partition->size){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "Update file exceeds partition capacity");
        return false; 
   }

   DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "OTA", "OTA passed file size size check");
   return true; 

}


//=============================================================================*
// OTA Diagnostic Functions
//=============================================================================*

void ota_print_update_partition(){

    // print the partition information that the update will be writen to. 

    const esp_partition_t* update_partition = get_ota_update_partition();

    if (update_partition == nullptr){
    
        DEBUG_PORT.println();
        DEBUG_PORT.println("OTA update partition: NONE");
    }
    else{
        DEBUG_PORT.println();
        DEBUG_PORT.println("========================================");
        DEBUG_PORT.println(" OTA UPDATE PARTITION INFORMATION ");
        DEBUG_PORT.println("========================================");

        DEBUG_PORT.println();
        DEBUG_PORT.println("OTA update partition:");

        DEBUG_PORT.print("  Label   : ");   
        DEBUG_PORT.println(update_partition->label);

        DEBUG_PORT.print("  Address : 0x");
        DEBUG_PORT.println(update_partition->address, HEX);

        DEBUG_PORT.print("  Size    : ");
        DEBUG_PORT.println(update_partition->size);

        DEBUG_PORT.print("  Type    : ");
        DEBUG_PORT.println(update_partition->type);

        DEBUG_PORT.print("  Subtype : 0x");
        DEBUG_PORT.println(update_partition->subtype, HEX);
    }
}

void ota_print_patrition_data(){

    DEBUG_PORT.println();
    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" OTA PARTITION INFORMATION ");
    DEBUG_PORT.println("========================================");

    /*-------------------------------------------------------------------------*
    *
    * Flash information
    *
    * ESP.getFlashChipSize() asks the ESP32 for the total size of the physical
    * flash chip.
    *
    * This is NOT the size available to the application. The flash is divided
    * into several partitions such as:
    *
    *   - NVS
    *   - OTA data
    *   - Application A
    *   - Application B
    *   - Filesystem
    *
    * We are interested in the complete flash size so we can see how the
    * partition table fits within the physical device.
    *
    *-------------------------------------------------------------------------*/

    DEBUG_PORT.printf("Flash size: %u bytes: \n", ESP.getFlashChipSize());

    /*-------------------------------------------------------------------------*
    * Running partition
    *
    * The ESP32 can have more than one application partition when OTA is
    * enabled.
    *
    * esp_ota_get_running_partition() returns a pointer to the partition
    * containing the firmware that is currently executing.
    *
    * For example:
    *  - ota_0 = Application A
    *  - ota_1 = Application B
    *
    * If the ESP32 is currently running ota_0, this function will return
    * information describing ota_0.
    *
    *------------------------------------------------------------------------*/

    // fetch the runnuing partition 
    const esp_partition_t* running_partition = esp_ota_get_running_partition();

    // Check that the running partition was returned before accessing it. 
    if(running_partition != nullptr){

        DEBUG_PORT.println("\nRunning_partition:");

        // Human readable name assigned eg. ota_0, ota_1, nvs, Spiffs/ LittleFS
        DEBUG_PORT.print("  Label   : ");
        DEBUG_PORT.println(running_partition->label);

        // The address is the location of the partition within the ESP32's flash memory.
        DEBUG_PORT.print("  Address : 0x");
        DEBUG_PORT.println(running_partition->address, HEX);

        // Size is the total amount of flash allocated to this partition.
        DEBUG_PORT.print("  Size    : ");
        DEBUG_PORT.println(running_partition->size);

        // Type identifies what kind of partition this is. 0 = Application 1 = Data.
        DEBUG_PORT.print("  Type    : ");
        DEBUG_PORT.println(running_partition->type);

        // Subtype gives more specific information about the partition such as OTA_1 / OTA_2
        DEBUG_PORT.print("  Subtype : 0x"); 
        DEBUG_PORT.println(running_partition->subtype, HEX);

    }

    /*-------------------------------------------------------------------------*
    * Boot partition
    *
    * The boot partition is the application partition that the ESP32 bootloader
    * has selected to run after the next reset.
    *
    * This is slightly different from the "running partition".
    *
    * Normally they will be the same, but during an OTA operation it is useful
    * to know what the bootloader has been instructed to run next.
    * 
    *-------------------------------------------------------------------------*/

    // Fetch the next booting partition 
    const esp_partition_t* boot_partition = esp_ota_get_boot_partition();

    // Check that the booting partition was returned before accessing it. 
    if(boot_partition != nullptr){

        DEBUG_PORT.println("\nboot_partition:");

        // Human readable name assigned eg. ota_0, ota_1, nvs, Spiffs/ LittleFS
        DEBUG_PORT.print("  Label   : ");
        DEBUG_PORT.println(boot_partition->label);
        
        // The address is the location of the partition within the ESP32's flash memory.
        DEBUG_PORT.print("  Address : 0x");
        DEBUG_PORT.println(boot_partition->address, HEX);

        // Size is the total amount of flash allocated to this partition.
        DEBUG_PORT.print("  Size    : ");
        DEBUG_PORT.println(boot_partition->size);

        // Type identifies what kind of partition this is. 0 = Application 1 = Data.
        DEBUG_PORT.print("  Type    : ");
        DEBUG_PORT.println(boot_partition->type);

        // Subtype gives more specific information about the partition such as OTA_1 / OTA_2
        DEBUG_PORT.print("  Subtype : 0x"); 
        DEBUG_PORT.println(boot_partition->subtype, HEX);
    }

    /*-------------------------------------------------------------------------*
    * Full partition table
    *
    * So far we have only looked at the application partitions.
    *
    * The ESP32 partition table also contains other partitions, such as NVS,
    * OTA data and the filesystem.
    *
    * esp_partition_find() allows us to ask the ESP32 for every partition in
    * the partition table.
    *
    * ESP_PARTITION_TYPE_ANY and ESP_PARTITION_SUBTYPE_ANY mean:
    *
    *   "Don't filter the results. Give me everything."
    *
    * nullptr for the label means we are not looking for a specific partition
    * name either.
    *-------------------------------------------------------------------------*/

    DEBUG_PORT.println();
    DEBUG_PORT.println("All partitions");
    DEBUG_PORT.println();

    // The iterator is used to walk through the partitions one at a time.
    esp_partition_iterator_t iterator = nullptr; 

    
    iterator = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, nullptr);

    // Walk through every partition returned by the ESP32 untill nullptr is received. 
    while(iterator != nullptr){

        // Convert the iterator into a pointer to the actual partition information.
        const esp_partition_t* partition = esp_partition_get(iterator); 

        // Print the useful information about this partition.
        DEBUG_PORT.printf("  %-12s | Type: %u | Subtype: 0x%02X | Address: 0x%08X | Size: %u bytes\n",
            partition->label,
            partition->type,
            partition->subtype,
            partition->address,
            partition->size
        );

        // Move the iterator to the next partition, if there are no more partitions, this will return nullptr. 
        iterator = esp_partition_next(iterator);  
    }

    // The iterator is an ESP32 framework resource, so it should be release onced finished with.
    esp_partition_iterator_release(iterator); 

    DEBUG_PORT.println();
    DEBUG_PORT.println("========================================");

}