
#include <Arduino.h>
#include "esp_ota_ops.h"
#include "debug.h"
#include "esp_ota_update.h"

#define DEBUG_FILE DBG_ESP_OTA

//=============================================================================*
// Function declarations
//=============================================================================*

static void print_flash_info();
static void print_running_partition();
static void print_boot_partition();
static void print_update_partition();
static void print_all_partitions();

static bool check_esp_ota_update_file_size(const esp_partition_t* partition, size_t firmware_size);
static const esp_partition_t* get_esp_ota_update_partition();

//=============================================================================*
// OTA Variables
//=============================================================================*

static esp_ota_handle_t ota_handle = 0;                       // Handle used to carry out the OTA update. 
static const esp_partition_t* ota_partition = nullptr;        // Cache the partition pointer 


//=============================================================================*
// OTA Update Sub Functions
//=============================================================================*

bool esp_update_begin(size_t firmware_size){

    // cache the partition being used for this OTA operation.
    ota_partition = get_esp_ota_update_partition();

    // Check that an OTA partition is available.
    if(ota_partition == nullptr){
        return false;
    }

    // Check that the firmware will fit within the OTA partition.
    if(!check_esp_ota_update_file_size(ota_partition, firmware_size)){
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


bool esp_update_write(const uint8_t* data, size_t length){

    esp_err_t result = esp_ota_write(ota_handle, data, length); 

    if(result != ESP_OK){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "OTA update failed to write to partition");
        return false; 
    }

    return true; 
}


bool esp_update_finalise(){

    esp_err_t result = esp_ota_end(ota_handle); 

    if(result != ESP_OK){ 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "OTA image failed to successfully verify");
        return false; 
    }

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "OTA", "OTA write finalised");

    ota_handle = 0; // Reset the handle variable.

    return true;

}

bool esp_update_abort(){ 
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

bool esp_update_set_boot_partition(){

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
    
    ota_partition = nullptr;  // Clear the Cached pointer

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "OTA", "OTA boot partition updated sucsessfully");
    return true; 

}

void esp_update_reboot(){

    // change to info once the the web interface reports the rebooting messae
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "OTA", "Rebooting system");   
    
    // reboot the ESP
    ESP.restart();
}


//=============================================================================*
// OTA Internal Functions
//=============================================================================*

static const esp_partition_t* get_esp_ota_update_partition(){

    return esp_ota_get_next_update_partition(nullptr); 

} 


static bool check_esp_ota_update_file_size(const esp_partition_t* partition, size_t firmware_size){

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
// ESP OTA Diagnostic Functions NEW 
//=============================================================================*

void esp_ota_print_diagnostics(){

    print_flash_info();
    print_running_partition();
    print_boot_partition();
    print_update_partition();
    print_all_partitions();
}


//=============================================================================*
// ESP OTA Diagnostic sub Functions NEW 
//=============================================================================*


static void print_flash_info(){
    DEBUG_PORT.println();
    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" FLASH INFORMATION ");
    DEBUG_PORT.println("========================================");

    DEBUG_PORT.print("Flash size: ");
    DEBUG_PORT.print(ESP.getFlashChipSize());
    DEBUG_PORT.println(" bytes");

}


static void print_running_partition(){

    const esp_partition_t* running_partition = esp_ota_get_running_partition();

    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" RUNNING PARTITION INFORMATION ");
    DEBUG_PORT.println("========================================");

    DEBUG_PORT.println("Running partition:");

    if(running_partition == nullptr){
        DEBUG_PORT.println("  NONE");
        return;
    }

    DEBUG_PORT.print("  Label   : ");
    DEBUG_PORT.println(running_partition->label);

    DEBUG_PORT.print("  Address : 0x");
    DEBUG_PORT.println(running_partition->address, HEX);

    DEBUG_PORT.print("  Size    : ");
    DEBUG_PORT.println(running_partition->size);

    DEBUG_PORT.print("  Type    : ");
    DEBUG_PORT.println(running_partition->type);

    DEBUG_PORT.print("  Subtype : 0x");
    DEBUG_PORT.println(running_partition->subtype, HEX);
}


static void print_boot_partition(){

    const esp_partition_t* boot_partition = esp_ota_get_boot_partition();

    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" BOOT PARTITION INFORMATION ");
    DEBUG_PORT.println("========================================");

    DEBUG_PORT.println("Boot partition:");

    if(boot_partition == nullptr){
        DEBUG_PORT.println("  NONE");
        return;
    }

    DEBUG_PORT.print("  Label   : ");
    DEBUG_PORT.println(boot_partition->label);

    DEBUG_PORT.print("  Address : 0x");
    DEBUG_PORT.println(boot_partition->address, HEX);

    DEBUG_PORT.print("  Size    : ");
    DEBUG_PORT.println(boot_partition->size);

    DEBUG_PORT.print("  Type    : ");
    DEBUG_PORT.println(boot_partition->type);

    DEBUG_PORT.print("  Subtype : 0x");
    DEBUG_PORT.println(boot_partition->subtype, HEX);

}


static void print_update_partition(){

    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(nullptr);

    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" UPDATE PARTITION INFORMATION ");
    DEBUG_PORT.println("========================================");

    DEBUG_PORT.println("Update partition:");

    if(update_partition == nullptr){
        DEBUG_PORT.println("  NONE");
        return;
    }

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


static void print_all_partitions(){

    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println(" ALL PARTITION DATA ");
    DEBUG_PORT.println("========================================");
    DEBUG_PORT.println("All partitions:");

    esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, nullptr);

    while(iterator != nullptr){

        const esp_partition_t* partition = esp_partition_get(iterator);

        DEBUG_PORT.printf(
            "  %-12s | Type: %u | Subtype: 0x%02X | Address: 0x%08X | Size: %u bytes\n",
            partition->label,
            partition->type,
            partition->subtype,
            partition->address,
            partition->size
        );

        iterator = esp_partition_next(iterator);
    }

    DEBUG_PORT.println();
    DEBUG_PORT.println();
}

