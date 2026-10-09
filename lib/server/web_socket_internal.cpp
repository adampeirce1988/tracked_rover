
#include <Arduino.h>
#include <stdint.h>

#include "debug.h" 

#include "esp_http_server.h"
// #include "websocket_internal.h"

//=============================================================================*
// Debug
//=============================================================================*

#define DEBUG_FILE DBG_WIFI

//=============================================================================*
// Internal Functions
//=============================================================================*

static esp_err_t websocket_handler(httpd_req_t *request);

//=============================================================================*
// const expressions (CONFIG) 
//=============================================================================*

constexpr uint16_t web_socket_port_number =  81;      // Defines the port no of the web socket. 
constexpr size_t max_message_size =          256;     // Max lengh of websocket payload 

//=============================================================================*
// Variables
//=============================================================================*

static httpd_handle_t websocket_server = NULL;

//=============================================================================*
// WebSocket Initialisation
//=============================================================================*

void websocket_init()
{
    // Create the WebSocket server using the ESP-IDF default configuration.
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Run the WebSocket server on port 81.
    // The existing Arduino WebServer remains on port 80.
    config.server_port = web_socket_port_number;

    // Start the ESP-IDF HTTP server.
    if (httpd_start(&websocket_server, &config) != ESP_OK)
    {
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_ERROR, "WSCK","Failed to start WebSocket server. Port: ", web_socket_port_number);
        return;
    }
    
        DEBUG_PRINT_MSG_VAL(DEBUG_FILE, DEBUG_INFO, "WSCK", "WebSocket server started. Port: ", web_socket_port_number);

    // Define the WebSocket endpoint.
    static const httpd_uri_t websocket_uri =
    {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = websocket_handler,
        .user_ctx = NULL,
        .is_websocket = true
    };

    // Register the WebSocket endpoint with the server.
    if(httpd_register_uri_handler(websocket_server, &websocket_uri) != ESP_OK){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to register WebSocket endpoint");
        return; 
    }

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WSCK", "WebSocket endpoint registered");
}

//=============================================================================*
// WebSocket Runtime
//=============================================================================*

void websocket_run()
{
    // The ESP-IDF HTTP server handles WebSocket communication
    // from its own task. No polling is required here at present.
}


//=============================================================================*
// WebSocket handler
//=============================================================================*
static esp_err_t websocket_handler(httpd_req_t *request){

    //===========================*
    // WebSocket Handshake
    //===========================*
    // The HTTP GET request is used to establish the WebSocket connection.
    if(request->method == HTTP_GET){

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WSCK", "WebSocket client connected");
        return ESP_OK;
    }

    //===========================*
    // Receive Frame Information
    //===========================*
    httpd_ws_frame_t frame = {};
        
    // Read the frame metadata before allocating memory for its payload.
    if(httpd_ws_recv_frame(request, &frame, 0) != ESP_OK){

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to receive WebSocket frame");
        return ESP_FAIL;
    }

    // Only accept text messages for this initial test.
    if (frame.type != HTTPD_WS_TYPE_TEXT)
    {
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_WARN, "WSCK", "Unsupported WebSocket frame type");
        return ESP_OK;
    }

    // Reject empty messages and messages larger than our test limit.
    if(frame.len == 0 || frame.len > max_message_size){ 
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_WARN, "WSCK", "Invalid WebSocket message size");
        return ESP_OK;
    }

    //===========================*
    // Receive Frame Payload
    //===========================*

    // Allocate space for the incoming message and a null terminator.
    uint8_t payload[max_message_size + 1] = {}; 

    // Receive the WebSocket frame payload.
    frame.payload = payload; 

    if(httpd_ws_recv_frame(request, &frame, frame.len) != ESP_OK){

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to receive WebSocket payload");
        return ESP_FAIL;
    }

    // Add a null terminator so the text can be logged safely.
    payload[frame.len] = '\0'; 

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WSCK", "Received WebSocket message"); 


    //===========================*
    // Send Test Reply
    //===========================*

    // prepare reply 
    static const char reply[] = "Hello from ESP32";

    httpd_ws_frame_t response = {};
    response.type = HTTPD_WS_TYPE_TEXT;
    response.payload = reinterpret_cast<uint8_t *>(const_cast<char *>(reply));
    response.len = sizeof(reply) - 1;

    // Send the reply to the connected browser.
    if (httpd_ws_send_frame(request, &response) != ESP_OK)
    {
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to send WebSocket reply");
        return ESP_FAIL;
    }

    return ESP_OK;
  
}