
#include <WiFi.h>
#include <Arduino.h>
#include <LittleFS.h>
#include <WebServer.h>
#include "debug.h"
#include "credentials.h"
#include "network_types.h"
  


/*=============================================================================*
 * Debug Configuration
*=============================================================================*/

#define DEBUG_FILE DBG_WIFI 


/*=============================================================================*
 * WIFI configuration 
*=============================================================================*/

constexpr uint8_t max_connection_attemps = 10;   // max connection atempts 


/*=============================================================================*
 * Variabes 
*=============================================================================*/ 
WebServer server(80);

/*-------------------------------------------------------------------------*  
  * Setup wifi 
*-------------------------------------------------------------------------*/
WIFI_STATUS wifi_init(){

    WIFI_STATUS wifi_status     = WIFI_STATUS::FAILED; 
    uint8_t connection_attemps  = 0;
    bool AP_mode_status         = false; 

    // connecting to prconfigured wifi 
    DEBUG_PORT.println("WiFi initlising . . ."); 
    DEBUG_PORT.print("Attempting to connect to: "); 
    DEBUG_PORT.println(WIFI_SSID);
    DEBUG_PORT.print("Connecting:"); 

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 

    while(WiFi.status() != WL_CONNECTED  && connection_attemps < max_connection_attemps){

        // print for visual progress
        DEBUG_PORT.print(" .");
        connection_attemps ++;
        delay(500); 
    }

    if(WiFi.status() == WL_CONNECTED){
        // print connection details
        DEBUG_PORT.print("connectio to: ");
        DEBUG_PORT.println(WIFI_SSID);
        DEBUG_PORT.print("IP address: ");
        DEBUG_PORT.println(WiFi.localIP());

        wifi_status = WIFI_STATUS::STA_MODE; 

    }
    else{ 

        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WIFI", "Failed to connecto to: ", WIFI_SSID);

        DEBUG_PORT.print("Switching to access point mode!"); 

        WiFi.mode(WIFI_AP); 
        AP_mode_status = WiFi.softAP(WIFI_SSID_AP_MODE);
        
        if(AP_mode_status){

            // connection sucsessful print details
            DEBUG_PORT.println("WiFi Netwok started!");
            DEBUG_PORT.print("Connected to: ");
            DEBUG_PORT.println(WIFI_SSID_AP_MODE);
            DEBUG_PORT.print("ESP32 IP address: ");
            DEBUG_PORT.println(WiFi.softAPIP());

            wifi_status = WIFI_STATUS::AP_MODE;
        }
        else{
            // repoert and connection error 
            DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WIFI", "Failed to connecto to: ", WIFI_SSID_AP_MODE);
        }
    }

    return wifi_status; 
}

/*-------------------------------------------------------------------------*  
  * Setup Server 
*-------------------------------------------------------------------------*/
void server_init(){

     server.on("/", HTTP_GET, []() {

        File file = LittleFS.open("/index.html", "r"); // open index.html

        // manage file opening failures 
        if (!file)
        {
            server.send(500, "text/plain", "Failed to open index.html");
            return;
        }

        server.streamFile(file, "text/html");
        file.close();

    });

     server.on("/style.css", HTTP_GET, []()
    {
        File file = LittleFS.open("/style.css", "r");

        if (!file)
        {
            server.send(500, "text/plain", "Failed to open style.css");
            return;
        }

        server.streamFile(file, "text/css");

        file.close();
    });

    // Register a GET endpoint for "/status".
    // When the web interface requests this URL, the lambda sends a JSON status response.
    // This is a place holder for furture expansion when i need to send data to the web page. 
    server.on("/status", HTTP_GET, []() {
        server.send(200, "application/json",
                    "{\"status\":\"ok\"}");
    });

    server.begin();

    Serial.println("HTTP server started");
}

/*-------------------------------------------------------------------------*  
  * Setup Server 
*-------------------------------------------------------------------------*/
void run_server(){
    server.handleClient();
}