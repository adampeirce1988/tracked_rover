#include <Arduino.h>
#include <LittleFS.h>
#include <WebServer.h>
#include "server_internal.h"
#include "system.h"
#include "system_types.h"
#include "server.h"
#include "debug.h"
#include "update.h"


//=============================================================================*
// Debug Configuration
//=============================================================================*
#define DEBUG_FILE DBG_WIFI


//=============================================================================*
// internal_functions 
//=============================================================================*

static void  register_home_route();
static void  register_update_route(); 
static void  register_diagnostics_route(); 
static void  register_sensor_data_route();  
static void  register_settings_route(); 

static void  register_css_route();
static void  register_js_route();

static void  register_status_route();

static void  register_firmware_upload_route();

//=============================================================================*
// variables
//=============================================================================*
WebServer server(80);

size_t received_byte_count = 0; 
size_t firmware_upload_size = 0;
bool firmware_upload_failed = false;


//=============================================================================*
// Server Initialisation
//=============================================================================*

void server_init(){
    // web pages 
    register_home_route();
    register_update_route(); 
    register_diagnostics_route(); // not yet implimented 
    register_sensor_data_route(); // page no yet implimented but included in the menue 
    register_settings_route(); // page no yet implimented but included in the menue 

    // static files
    register_css_route();
    register_js_route();

    // JSON file handler
    register_status_route();
    // functions 
    register_firmware_upload_route();


    // start the server
    server.begin();

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "SERV", "HTTP server initiataed"); // change to info after debug

}

//=============================================================================*
// Server Runtime
//=============================================================================*

void server_run(){
    server.handleClient();
}

//=============================================================================*
// Web Page Routes
//=============================================================================*


//--------------------------*
// Home Route Handler
//--------------------------*
static void register_home_route(){

    server.on("/", HTTP_GET, []() {

    File file = LittleFS.open("/index.html", "r"); // open index.html

    // manage file opening failures 
    if (!file){
        server.send(500, "text/plain", "Failed to open index.html");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();

    });
}


//--------------------------*
// Diagnostics root handler
//--------------------------*
static void register_diagnostics_route(){
    server.on("/diagnostics", HTTP_GET, [](){

        File file = LittleFS.open("/diagnostics.html", "r");

        if(!file){
            server.send(200, "text/plain", "failed to open Diagostics.html"); 
            return; 
        }

        server.streamFile(file, "text/html");

    });
}


//--------------------------*
// Update Root Handler 
//--------------------------*
static void register_update_route(){

    server.on("/update.html", HTTP_GET, []() {

        STATE_CHANGE_RETURN_CODE result = request_vehicle_state_change(VEHICLE_STATE::UPDATE); 

        // debug 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "UDAT", "Update state requested");

        if(result == STATE_CHANGE_RETURN_CODE::APPROVED){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "UDAT", "Update state Approved");
        }
        else if(result == STATE_CHANGE_RETURN_CODE::DENIED){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "UDAT", "Update state Denied");
        }
        else if(result == STATE_CHANGE_RETURN_CODE::UNCHANGED){
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "UDAT", "Update state Unchanged");
        }

        File file = LittleFS.open("/update.html", "r"); 

        if(!file){
            server.send(500, "text/plain", "failed to open update.html");
            return; 
        }

        server.streamFile(file, "text/html");

        file.close();
    });

}
//--------------------------*
// sensor data
//--------------------------*
static void register_sensor_data_route(){

}
//--------------------------*
// settings route
//--------------------------*
static void register_settings_route(){

}


//=============================================================================*
// Static File Routes
//=============================================================================*

//--------------------------*
// CSS file
//--------------------------*
static void register_css_route(){

    server.on("/style.css", HTTP_GET, []() {
        File file = LittleFS.open("/style.css", "r");

        if (!file)
        {
            server.send(500, "text/plain", "Failed to open style.css");
            return;
        }

        server.streamFile(file, "text/css");

        file.close();
    });

}

//--------------------------*
// javascript file
//--------------------------*
static void register_js_route(){

    server.on("/scripts.js", HTTP_GET, []() {

        File file = LittleFS.open("/scripts.js", "r");

        if(!file){
            server.send(500, "text/plain", "Failed to open scripts.js");
            return;
        }

        server.streamFile(file, "application/javascript");

        file.close();
    });   
    
}


//=============================================================================*
// JSON file handler Route
//=============================================================================*

static void register_status_route(){

    server.on("/status", HTTP_GET, [](){

        const char* current_vehicle_state = get_active_state_as_string(); 
        
        String response = "{\"vehicle_state\":\""; response += current_vehicle_state; response += "\"}";

        server.send(200, "application/json",response); 
    });
}


//=============================================================================*
// Firmware Update Route
//=============================================================================*

static void register_firmware_upload_route(){

    server.on("/update-firmware", HTTP_POST, [](){

        server.send(200, "text/plain", "Firmware upload received");
    
    }, 


    [](){ HTTPUpload &upload = server.upload(); 

        if(upload.status == UPLOAD_FILE_START){

            // Always clear at the start of an update.
            firmware_upload_failed = false;
                
            if(!server.hasArg("size")){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Firmware upload size not provided");
                firmware_upload_failed = true; 
                return; 
            }

            // Store the file size received from the client. 
            size_t update_size = server.arg("size").toInt();

            // Check for files that contain no data.
            if(update_size == 0){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Invalid firmware upload size");
                firmware_upload_failed = true; 
                return; 
            }

            if(!update_begin(UPDATE_TYPE::FIRMWARE, update_size)){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Failed to begin firmware update");
                firmware_upload_failed = true; 
                return; 
            } 

            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "UDAT", "Firmware update started");

        }
        else if(upload.status == UPLOAD_FILE_WRITE){
            // Pass the received chunk to the update module.
            if(!update_write(upload.buf, upload.currentSize)){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Failed to write firmware update data");
                firmware_upload_failed = true; 
                update_abort(); 
            }
        }
        else if(upload.status == UPLOAD_FILE_END){

            if(firmware_upload_failed){
                return; 
            }

            if(!update_finalise()){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Failed to finalise firmware update");
                firmware_upload_failed = true; 
                update_abort();
            }
        
        }
        else if(upload.status == UPLOAD_FILE_ABORTED){
            // The HTTP upload itself was interrupted.
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "UDAT", "Firmware upload aborted");
            firmware_upload_failed = true; 
            update_abort();
        }
        

    });
}
