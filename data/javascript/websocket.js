
//=============================================================================*
// ESP-Nexus WebSocket Module
//=============================================================================*

console.log("websocket.js loaded"); 


//=============================================================================*
// Configuration
//=============================================================================*

const WS_CONFIG = {
    url: "ws://" + window.location.hostname + ":81/ws",
    reconnectDelayMs: 1000,
    lowSpeedTelemetryStaleAfterMs: 550,  // double packe speed pluss 10%
    highSpeedTelemetryStaleAfterMs: 220  // double packe speed pluss 10%
};

//=============================================================================*
// Internal State
//=============================================================================*

let vehicleSocket = null;               // Stores the websocket used for telemetry.
let connectionState = "disconnected";   // Tracks the connection state. connecting, connected or disconnected.
let lastHighSpeedTimestamp = null;      // stores a timestamp for the last high speed packet received.
let lastLowSpeedTimestamp = null;       // stores a timestamp for the last low speed packet received.
const latestValues = {};                // store the latest values sent over JSON from the ESP32.
const telemetryListeners = new Set();   // Stores callbacks that are notified when telemetry changes.

//=============================================================================*
// WebSocket Connection
//=============================================================================*

function connectWebSocket(){

    // prevent duplicated connection if a WebScoket alreay exsists.
    if(vehicleSocket != null){
        return; 
    }

    // Update the connection state before attempting to create the socket.
    connectionState = "connecting"; 

    // Connect to the ESP-32's WebSocket server.
    vehicleSocket = new WebSocket(WS_CONFIG.url); 

    // update the connectiom state once the websocket opens.
    vehicleSocket.onopen = function(){
        connectionState = "connected"; 
        console.log("ESP-Nexus webSocket connected");   
    };

    // Clear the closed socket and schedule an automatic reconnect.
    vehicleSocket.onclose = function(){
        connectionState = "disconnected"; 
        vehicleSocket = null; 
        console.log("ESP-Nexus webSocket disconnected");

        // attempt to reconnect after the configured delay.
        setTimeout(function() {
            connectWebSocket();
        }, WS_CONFIG.reconnectDelayMs);
    };

    // Parse received JSON text into a javaScrip object.
    vehicleSocket.onmessage = function(event){

        let data; 

        // converthe the received JSON into a Javascript object
        try {
            data = JSON.parse(event.data); 
        } 
        catch (error) {
            console.error("Invalid WebSocket JSON:", error);
            return; 
        }

        // Reject null arrays and non-object values before updating the telemetry 
        if(data === null || typeof data !== "object" || Array.isArray(data)){
            consle.error("Invalid telemetry format: expected a JSON object"); 
            return; 
        }

        // store the last updated telemetry packet time
        const messageTime = Date.now();

        if(data.type === "hs_telemetry"){
            lastHighSpeedTimestamp = messageTime;
        }
        else if(data.type === "ls_telemetry"){
            lastLowSpeedTimestamp = messageTime;
        }
        else{
            console.warn("Ingoring unknown telemetry message type: ", data.type);
            return; 
        }
        

        // Merge new fields without removing values from previous messages.
        Object.assign(latestValues, data);

        // notify the subscriber that the telemetry has changed
        notifyTelemetryListeners();

        // Log the updated telemetry store. 
        //console.log("ESP-Nexus telemetry reveived:" ,latestValues);  // uncommet for visable logs 
    };

    // Log socket errors (reconnect is handled onclose). 
    vehicleSocket.onerror = function(error){
        console.error("ESP-Nexus websocket error: ",error); 
        return; 
    };

}


//=============================================================================*
// WebSocket functions
//=============================================================================*



function testWebSocketReconnect(){

    // close the socket to test automatic reconnection 
    if(vehicleSocket !== null){
            vehicleSocket.close()
    }
}


function getConectionStateInternal(){

    return connectionState;
    
}


function notifyTelemetryListeners(){

    // notify each telemetry listener with the latetst telemetry values
    telemetryListeners.forEach(function(callback) {

        callback({...latestValues });
    });

}


function subscribeTelemetry(callback) {

      // Reject invalid subscribers to prevent errors when notifying listeners.
    if( typeof callback !== "function"){
        console.error("Telemerty subscriber must not be a function")
        return function() {};
    }

    telemetryListeners.add(callback); 

    // Return a function that allows this subscriber to be removed.
    return function(){
        telemetryListeners.delete(callback); 
    }; 
}


function unsubscribeTelemetry(callback){
    // remove the subscriber.
    telemetryListeners.delete(callback);
}


function getTelemetryValue(name){
    return latestValues[name];
}


function getAllTelemetryValues(){
    return { ...latestValues }
}


function getHighSpeedTelemetryHealth(){
    return lastHighSpeedTimestamp !== null && Date.now() - lastHighSpeedTimestamp <= WS_CONFIG.highSpeedTelemetryStaleAfterMs;
}

function getLowSpeedTelemetryHealth(){
    return lastLowSpeedTimestamp !== null && Date.now() - lastLowSpeedTimestamp <= WS_CONFIG.lowSpeedTelemetryStaleAfterMs;
}

function getTelemetryHealth(){

    return{
        connectionState: connectionState,
        highSpeed: getHighSpeedTelemetryHealth(),
        lowSpeed: getLowSpeedTelemetryHealth()
    };

}



//=============================================================================*
// Public Interface
//=============================================================================*

const VehicleTelemetry = {

    connect: connectWebSocket, 
    testReconnect: testWebSocketReconnect,
    subscribe: subscribeTelemetry,
    unsubscribe:unsubscribeTelemetry,
    getConnectionState: getConectionStateInternal,
    getValue: getTelemetryValue,
    getAllValues: getAllTelemetryValues,
    getHealth: getTelemetryHealth,
    getTelemetryHShealth: getHighSpeedTelemetryHealth, 
    getTelemetryLSHealth: getLowSpeedTelemetryHealth,
    

    // rundiagnostic test suite 
    runTest: runVehicleTelemetryTests
};

window.VehicleTelemetry = VehicleTelemetry;




/*=============================================================================*
 * Browser Diagnostics
 *
 * Run the WebSocket test suite from the browser console:
 *     VehicleTelemetry.runTest();
 *
 * Tests the public API, connection handling, high-speed and low-speed
 * telemetry, cache merging, subscribers, channel freshness and reconnection.
 *
 * Results are logged to the console and returned as a summary object.
 *=============================================================================*/

async function runVehicleTelemetryTests() {

    const results = [];
    const timeoutMs = 4000;

    // Allow enough time for both channels to exceed their stale thresholds.
    const staleWaitMs = Math.max(
        WS_CONFIG.highSpeedTelemetryStaleAfterMs,
        WS_CONFIG.lowSpeedTelemetryStaleAfterMs
    ) + 150;

    // Wait until a condition becomes true or the timeout expires.
    async function waitFor(condition, timeout = timeoutMs) {

        const start = Date.now();

        while (Date.now() - start < timeout) {

            if (condition()) {
                return true;
            }

            await new Promise(resolve => setTimeout(resolve, 50));
        }

        throw new Error("Timed out waiting for condition");
    }

    // Run one test and record its result without stopping the suite.
    async function runTest(name, testFunction) {

        try {
            await testFunction();

            results.push({
                Test: name,
                Result: "PASS"
            });

            console.log("PASS:", name);
        }
        catch (error) {

            results.push({
                Test: name,
                Result: "FAIL",
                Error: error.message
            });

            console.error("FAIL:", name, error);
        }
    }

    // Wait for a particular telemetry channel to send an update.
    async function waitForTelemetryType(type, timeout = timeoutMs) {

        let unsubscribe;

        const messageReceived = new Promise((resolve, reject) => {

            const timer = setTimeout(() => {

                if (unsubscribe) {
                    unsubscribe();
                }

                reject(new Error("Timed out waiting for " + type));
            }, timeout);

            unsubscribe = VehicleTelemetry.subscribe(data => {

                if (data.type === type) {

                    clearTimeout(timer);
                    unsubscribe();
                    resolve(data);
                }
            });
        });

        return await messageReceived;
    }

    // Check that an object contains all the expected telemetry fields.
    function checkFields(values, fields, channelName) {

        fields.forEach(field => {

            if (!Object.prototype.hasOwnProperty.call(values, field)) {
                throw new Error(
                    channelName + " telemetry is missing field: " + field
                );
            }
        });
    }

    console.log("Starting VehicleTelemetry browser tests...");

    // 1. Check the public API.
    await runTest("Public API methods exist", async () => {

        const methods = [
            "connect",
            "testReconnect",
            "subscribe",
            "unsubscribe",
            "getConnectionState",
            "getValue",
            "getAllValues",
            "getHealth",
            "runTest"
        ];

        methods.forEach(method => {

            if (typeof VehicleTelemetry[method] !== "function") {
                throw new Error("Missing method: " + method);
            }
        });
    });

    // 2. Establish the WebSocket connection.
    await runTest("WebSocket connects", async () => {

        VehicleTelemetry.connect();

        await waitFor(() =>
            VehicleTelemetry.getConnectionState() === "connected"
        );
    });

    // 3. Calling connect again must be safe.
    await runTest("Repeated connect is safe", async () => {

        VehicleTelemetry.connect();
        VehicleTelemetry.connect();

        if (VehicleTelemetry.getConnectionState() !== "connected") {
            throw new Error("Connection was not maintained");
        }
    });

    // 4. Confirm high-speed telemetry is received.
    await runTest("High-speed telemetry is received", async () => {

        await waitForTelemetryType("hs_telemetry");

        if (!VehicleTelemetry.getHealth().highSpeed) {
            throw new Error("High-speed telemetry is not healthy");
        }
    });

    // 5. Confirm low-speed telemetry is received.
    await runTest("Low-speed telemetry is received", async () => {

        await waitForTelemetryType("ls_telemetry");

        if (!VehicleTelemetry.getHealth().lowSpeed) {
            throw new Error("Low-speed telemetry is not healthy");
        }
    });

    // 6. Check that high-speed telemetry is stored.
    await runTest("High-speed telemetry is stored", async () => {

        const values = VehicleTelemetry.getAllValues();

        checkFields(values, [
            "acc_x",
            "acc_y",
            "acc_z",
            "gyro_x",
            "gyro_y",
            "gyro_z"
        ], "High-speed");
    });

    // 7. Check that low-speed telemetry is stored.
    await runTest("Low-speed telemetry is stored", async () => {

        const values = VehicleTelemetry.getAllValues();

        checkFields(values, [
            "vehicle_status",
            "battery_voltage",
            "wifi_mode",
            "wifi_signal"
        ], "Low-speed");
    });

    // 8. Check that the two channels share the cache without overwriting
    //    each other's values.
    await runTest("Telemetry channels merge correctly", async () => {

        await waitForTelemetryType("hs_telemetry");

        const afterHighSpeed = VehicleTelemetry.getAllValues();

        checkFields(afterHighSpeed, [
            "acc_x",
            "gyro_x",
            "vehicle_status",
            "battery_voltage"
        ], "Merged");

        await waitForTelemetryType("ls_telemetry");

        const afterLowSpeed = VehicleTelemetry.getAllValues();

        checkFields(afterLowSpeed, [
            "acc_x",
            "gyro_x",
            "vehicle_status",
            "battery_voltage"
        ], "Merged");
    });

    // 9. Check that both channels report healthy while connected.
    await runTest("Both telemetry channels report healthy", async () => {

        await waitFor(() => {
            const health = VehicleTelemetry.getHealth();

            return health.connectionState === "connected" &&
                health.highSpeed === true &&
                health.lowSpeed === true;
        });

        const health = VehicleTelemetry.getHealth();

        if (health.highSpeed !== true || health.lowSpeed !== true) {
            throw new Error("One or both telemetry channels are unhealthy");
        }
    });

    // 10. A subscriber must receive telemetry updates.
    await runTest("Subscriber receives updates", async () => {

        let received = false;

        const unsubscribe = VehicleTelemetry.subscribe(() => {
            received = true;
        });

        try {
            await waitFor(() => received);
        }
        finally {
            unsubscribe();
        }
    });

    // 11. Unsubscribed listeners must stop receiving updates.
    await runTest("Unsubscribe stops notifications", async () => {

        let count = 0;

        const unsubscribe = VehicleTelemetry.subscribe(() => {
            count++;
        });

        unsubscribe();

        const countAfterUnsubscribe = count;

        await waitForTelemetryType("hs_telemetry");

        if (count !== countAfterUnsubscribe) {
            throw new Error("Subscriber received an update after unsubscribe");
        }
    });

    // 12. An invalid subscriber must not be registered.
    await runTest("Invalid subscriber is handled", async () => {

        const unsubscribe = VehicleTelemetry.subscribe(null);

        if (typeof unsubscribe !== "function") {
            throw new Error("Invalid subscriber did not return a cleanup function");
        }

        unsubscribe();
    });

    // 13. Closing the socket should update connection state.
    await runTest("Socket closes correctly", async () => {

        VehicleTelemetry.testReconnect();

        await waitFor(() =>
            VehicleTelemetry.getConnectionState() === "disconnected"
        );
    });

    // 14. With the socket closed, both channels should become stale.
    await runTest("Both telemetry channels become stale", async () => {

        await new Promise(resolve => setTimeout(resolve, staleWaitMs));

        const health = VehicleTelemetry.getHealth();

        if (health.connectionState !== "disconnected") {
            throw new Error("Socket reconnected before stale check");
        }

        if (health.highSpeed !== false) {
            throw new Error("High-speed telemetry was not marked stale");
        }

        if (health.lowSpeed !== false) {
            throw new Error("Low-speed telemetry was not marked stale");
        }
    });

    // 15. The socket should reconnect automatically.
    await runTest("Automatic reconnection works", async () => {

        await waitFor(
            () => VehicleTelemetry.getConnectionState() === "connected",
            timeoutMs + WS_CONFIG.reconnectDelayMs + 2000
        );
    });

    // 16. Both channels should recover after reconnection.
    await runTest("Telemetry recovers after reconnection", async () => {

        await waitFor(() => {
            const health = VehicleTelemetry.getHealth();

            return health.connectionState === "connected" &&
                health.highSpeed === true &&
                health.lowSpeed === true;
        });
    });

    // Display the final results.
    const passed = results.filter(
        result => result.Result === "PASS"
    ).length;

    const failed = results.length - passed;

    console.log(`RESULT: ${passed}/${results.length} passed`);

    if (failed > 0) {

        console.error(
            "Failed tests:",
            results.filter(result => result.Result === "FAIL")
        );
    }

    console.table(results);

    return {
        passed: passed,
        failed: failed,
        total: results.length,
        results: results
    };
}
