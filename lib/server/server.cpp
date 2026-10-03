#include <Arduino.h>
#include <LittleFS.h>
#include <WebServer.h>
#include "server_internal.h"
#include "server.h"
#include "debug.h"
#include "update.h"


//=============================================================================*
// Debug Configuration
//=============================================================================*
#define DEBUG_FILE DBG_WIFI


//=============================================================================*
// variables
//=============================================================================*
WebServer server(80);


//=============================================================================*
// Server Initialisation
//=============================================================================*

void server_init(){
    // web pages 
    register_home_route();
    register_diagnostics_route();
    register_sensor_data_route(); // page no yet implimented but included in the menue 
    register_settings_route(); // page no yet implimented but included in the menue 

    // static files
    register_css_route();
    register_js_route();

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

// Home Route Handler  
void register_home_route(){

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

// Diagnostic Root Handler 
void register_diagnostics_route(){

    server.on("/update.html", HTTP_GET, []() {
        File file = LittleFS.open("/update.html", "r"); 

        if(!file){
            server.send(500, "text/plain", "failed to open update.html");
            return; 
        }

        server.streamFile(file, "text/html");

        file.close();
    });

}

// sensor data
void register_sensor_data_route(){

}

// settings route
void register_settings_route(){

}


//=============================================================================*
// Static File Routes
//=============================================================================*

// css file
void register_css_route(){

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

// javascript file
void register_js_route(){

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
// Firmware Update Route
//=============================================================================*

void register_firmware_upload_route(){

    server.on("/update-firmware", HTTP_POST, [](){

        server.send(200, "text/plain", "Firmware upload received");
    
    }, 


    [](){ HTTPUpload &upload = server.upload(); 

        if(upload.status == UPLOAD_FILE_START){
            if(!update_begin(UPDATE_TYPE::FIRMWARE, upload.totalSize)){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WIFI", "Failed to begin firmware update");
                return; 
            } 

        }
        else if(upload.status == UPLOAD_FILE_WRITE){
            // Pass the received chunk to the update module.
            if(!update_write(upload.buf, upload.currentSize)){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WIFI", "Failed to write firmware update data");
                update_abort(); 
            }
        }
        else if(upload.status == UPLOAD_FILE_END){

            if(!update_finalise()){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WIFI", "Failed to finalise firmware update");
                update_abort();
            }
        }
        else if(upload.status == UPLOAD_FILE_ABORTED){
            // The HTTP upload itself was interrupted.
            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WIFI", "Firmware upload aborted");
            update_abort();
        }
        

    });
}