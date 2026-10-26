
//=============================================================================*
// ESP-Nexus WebSocket Module
//=============================================================================*

console.log("websocket.js loaded"); 


//=============================================================================*
// Configuration
//=============================================================================*

const WS_CONFIG = {
    url: "ws://" + window.location.hostname + ":81/ws",
    reconnectDelayMs: 2000
};

//=============================================================================*
// Internal State
//=============================================================================*

let vehicleSocket = null; 
let connectionState = "disconnected"; 
const latestValues = {};

//=============================================================================*
// WebSocket Connection
//=============================================================================*

function connectWebSocket(){

    // prevent creating a second active connection
    if(vehicleSocket != null){
        return; 
    }

    // Update the connection state before connecting.
    connectionState = "connecting"; 

    // establish the WebSocket connection 
    vehicleSocket = new WebSocket(WS_CONFIG.url); 

    // connection established
    vehicleSocket.onopen = function(){
        connectionState = "connected"; 
        console.log("ESP-Nexus webSocket connected"); 
    };

    //connection closed
    vehicleSocket.onclose = function(){
        connectionState = "disconnected"; 
        vehicleSocket = null; 
        console.log("ESP-Nexus webSocket disconnected");

        // attempt to reconnect after the configured delay
        setTimeout(function() {
            connectWebSocket();
        }, WS_CONFIG.reconnectDelayMs);
    };

    // connection error
    vehicleSocket.onerror = function(error){
        console.error("ESP-Nexus websocket error: ",error); 
    };

}

//=============================================================================*
// Public Interface
//=============================================================================*

const VehicleTelemetry = {
    connect: function(){
        connectWebSocket();
    },
    getConnectionState: function() {
        return connectionState;
    },

    getValue: function(name) {
        return latestValues[name];
    },

    getAllValues: function() {
        return { ...latestValues };
    }
};

window.VehicleTelemetry = VehicleTelemetry;