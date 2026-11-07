#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <atomic>

#include "esp_http_server.h"

#include "debug.h"
#include "system.h"


//=============================================================================*
// Debug
//=============================================================================*

#define DEBUG_FILE DBG_WIFI

//=============================================================================*
// Internal Functions
//=============================================================================*

static esp_err_t websocket_handler(httpd_req_t *request);

//=============================================================================*
// Websocket Configuration (CONFIG) 
//=============================================================================*

constexpr uint16_t web_socket_port_number =  81;      // Defines the port no of the web socket. 
constexpr size_t max_message_size =          256;     // Max lengh of websocket payload 
constexpr uint8_t max_conected_clients =     2;       // Max no of clients

// web socket ring buffer
constexpr size_t max_telemetry_packet_size =   512; // Max packet size refine later once telemetry is full assembled
constexpr uint8_t broadcast_queue_capacatiy =  4;   // max capacity of telemetry queue (reduce ince the max queue size is recorded)
constexpr uint8_t max_queue_attempts =         5;   // max amount of attemps to queue a message      

/*=============================================================================*
 * WebSocket Runtime State
*=============================================================================*/

// WebSocket server handle.
static httpd_handle_t websocket_server = NULL;

// Broadcast message structure.
struct websocket_broadcast_t {
    char message[max_telemetry_packet_size];
    size_t length;
};

// Broadcast queue.
static websocket_broadcast_t broadcast_queue[broadcast_queue_capacatiy];
static uint8_t broadcast_queue_head = 0;
static uint8_t broadcast_queue_tail = 0;
static uint8_t broadcast_queue_count = 0;

// Atomic flags for queue access and worker scheduling.
static std::atomic<bool> broadcast_queue_locked{false};
static std::atomic<bool> broadcast_pending{false};


/*=============================================================================*
 * Internal Function Declarations
*=============================================================================*/

// WebSocket handler.
static esp_err_t websocket_handler(httpd_req_t *request);

// Broadcast queue management.
static bool websocket_queue_lock();
static void websocket_queue_unlock();
static bool websocket_queue_message(const char* message);
static bool websocket_queue_pop(websocket_broadcast_t& message);

// Broadcast processing.
static void websocket_broadcast_worker(void* args);
static void websocket_schedule_broadcast();
static void websocket_send_message(const websocket_broadcast_t& message);


/*=============================================================================*
 * WebSocket Initialisation
*=============================================================================*/

void websocket_init()
{
    // Create the WebSocket server using the ESP-IDF default configuration.
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // configure the websocket
    config.server_port = web_socket_port_number;      // Run the WebSocket server on port (default 81)
    config.max_open_sockets = max_conected_clients;   // configure maximum allowed clients (default 2)

    // Start the ESP-IDF HTTP server.
    if (httpd_start(&websocket_server, &config) != ESP_OK){

        websocket_server = NULL;

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

        // Stop the server if endpoint registration fails.
        httpd_stop(websocket_server);
        websocket_server = NULL;

        return; 
    }

    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WSCK", "WebSocket endpoint registered");
}

/*=============================================================================*
 * WebSocket Runtime
*=============================================================================*/

void websocket_run()
{
    // The ESP-IDF HTTP server handles WebSocket communication
    // from its own task. No polling is required here at present.
}


/*=============================================================================*
 * WebSocket Handler
*=============================================================================*/

static esp_err_t websocket_handler(httpd_req_t *request){

    //===========================*
    // WebSocket Handshake
    //===========================*

    // debug to confirm that the handler is initaited
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_INFO, "WSCK", "WebSocket handler invoked");

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
    
    // Read the current vehicle state from the system FSM.
    const char* vehicle_state = get_active_state_as_string();

    // Format the vehicle status as a JSON message.
    char reply[96] = {};

    int reply_length = snprintf(reply, sizeof(reply), "{\"type\":\"status\",\"vehicle_state\":\"%s\"}", vehicle_state);  

     // Reject the reply if formatting failed or exceeded the buffer.
    if (reply_length < 0 || reply_length >= sizeof(reply)){

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to format vehicle status");
        return ESP_FAIL;
    }

    // Prepare the WebSocket response.
    httpd_ws_frame_t response = {};
    response.type = HTTPD_WS_TYPE_TEXT;
    response.payload = reinterpret_cast<uint8_t*>(reply);
    response.len = reply_length;

    // Send the vehicle status to the connected browser.
    if (httpd_ws_send_frame(request, &response) != ESP_OK)
    {
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK","Failed to send vehicle status");
        return ESP_FAIL;
    }

    return ESP_OK;
  
}


/*=============================================================================*
 * Broadcast Queue
*=============================================================================*/

// Add a message to the broadcast queue for transmission by the HTTP server task.
void web_socket_broadcast_text(const String message){

    // check that the server is running. 
    if(websocket_server == NULL || message == nullptr){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to boradcast telemety");
        return;
    }

    //attemp to queue the message up to max_attemps
    for(uint8_t attempts = 0; attempts < max_queue_attempts; attempts++ ){
        // add the message to the broadcast queue using c_str() to converto to char*. 
        if(websocket_queue_message(message.c_str())){

            websocket_schedule_broadcast();

            DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "WSCK", "WebSocket broadcast message queued"); // change to debug_info 
            return; 
        }

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_DEBUG, "WSCK", "Broadcast queue attempt failed"); 
    }

    // all attempts failed
    DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_WARN, "WSCK", "Failed to add message to broadcast queue"); 
}


// Copy a message into the next available queue slot.
// Returns false if the queue is locked, full or the message is invalid.
static bool websocket_queue_message(const char* message){

    // check if the queue is locked
    if(!websocket_queue_lock()){
        return false; 
        // return if the queue is locked
    }

    // protect against blank messages
    if(message == nullptr){

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_WARN, "WSCK", "Rejected message: nullptr");
        websocket_queue_unlock();
        return false; 
    }

    size_t message_length = strlen(message);

    if(message_length == 0 || message_length >= max_telemetry_packet_size){

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_WARN, "WSCK", "Rejected invalid broadcast message size");
        websocket_queue_unlock();
        return false; 
    }

    // drop the incomming message if the queue is full 
    if(broadcast_queue_count == broadcast_queue_capacatiy){

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Websocket broadcast queue full: incoming message droped");
        websocket_queue_unlock();
        return false; 
    }


    // copy new message into the next avaliable slot
    websocket_broadcast_t& slot = broadcast_queue[broadcast_queue_head];

    memcpy(slot.message, message, message_length);
    slot.message[message_length] = '\0'; 
    slot.length = message_length; 

    // advance the write possition
    broadcast_queue_head = (broadcast_queue_head + 1) % broadcast_queue_capacatiy;
    broadcast_queue_count ++; 

    websocket_queue_unlock();

    return true; 
}


// Retrieve the oldest pending message from the queue.
// Returns false if the queue is empty or currently locked.
static bool websocket_queue_pop(websocket_broadcast_t& message){

    if(!websocket_queue_lock()){
        // return if the queue is locked
        return false;
    }


    // check to see if the queue contains a message 
    if(broadcast_queue_count == 0){
        websocket_queue_unlock(); 
        return false; 
    }

    // coppy the oldest message out of the queue
    message = broadcast_queue[broadcast_queue_tail]; 

    broadcast_queue_tail = (broadcast_queue_tail+ 1) % broadcast_queue_capacatiy;
    broadcast_queue_count --; 

    // release exclusive access to the queue 
    websocket_queue_unlock();

    return true; 
}


// Attempt to acquire exclusive access to the broadcast queue.
// Returns true if the lock was acquired, or false if another task holds it.
static bool websocket_queue_lock(){
    bool expected = false; 

    return broadcast_queue_locked.compare_exchange_strong(expected, true, std::memory_order_acquire);
}

// Release exclusive access to the broadcast queue so another task can use it.
static void websocket_queue_unlock()
{
    broadcast_queue_locked.store(false, std::memory_order_relaxed);
}


/*=============================================================================*
 * Broadcast Worker
*=============================================================================*/

static void websocket_broadcast_worker(void* args){

    websocket_broadcast_t message; 


    while(true){

        // process all pending messages
        while(websocket_queue_pop(message)){
            websocket_send_message(message);
        }

        // release the pending flag 
        broadcast_pending.store(false); 

        // check if any massages arrived during the release of the pending flag
        if(!websocket_queue_pop(message)){
            return; 
        }
        
        // a message has arrive - continue processing the queue
        websocket_send_message(message);
    
    }

    // No messages currently remain. release the pending flag 
    broadcast_pending.store(false);
}


/*=============================================================================*
 * Broadcast Scheduling
*=============================================================================*/

static void websocket_schedule_broadcast(){

    // Check that the server is running.
    if(websocket_server == NULL){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "WebSocket server unavailable");
        return;
    }

    bool expected = false; 

    // Check whether the worker is already pending
    if(!broadcast_pending.compare_exchange_strong(expected, true)){
        return; 
    }


    // Queue the worker in the HTTP server task 
    if(httpd_queue_work(websocket_server, websocket_broadcast_worker, nullptr) != ESP_OK){
        
        // Clear the pending flag if scheduling fails.
        broadcast_pending.store(false); 

        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to schedule broadcast worker");
    }
}


/*=============================================================================*
 * WebSocket Transmission
*=============================================================================*/

static void websocket_send_message(const websocket_broadcast_t& message){


    // Check that the server is running.
    if(websocket_server == NULL){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "WebSocket server unavailable");
        return;
    }

    //preare the websocket frame
    httpd_ws_frame_t frame = {};
    frame.final = true; 
    frame.type = HTTPD_WS_TYPE_TEXT; 
    frame.payload = reinterpret_cast<uint8_t*>(const_cast<char*>(message.message)); 
    frame.len = message.length;

    // retreive the connected client_file_description
    int client_fds[max_conected_clients] ={};
    size_t client_count = max_conected_clients;

    if(httpd_get_client_list(websocket_server, &client_count, client_fds) != ESP_OK){
        DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_ERROR, "WSCK", "Failed to retreive client websocket clients");
        return; 
    }
    
    //send messages to all connected Websocket clients
    for(size_t i = 0; i < client_count; i++){
        if(httpd_ws_get_fd_info(websocket_server, client_fds[i]) != HTTPD_WS_CLIENT_WEBSOCKET){
            continue; 
        }
        
        if(httpd_ws_send_frame_async(websocket_server, client_fds[i], &frame) != ESP_OK){
                DEBUG_PRINT_MSG(DEBUG_FILE, DEBUG_WARN, "WSCK", "Failed to broadcast WebSocket message");
        }
    }
}