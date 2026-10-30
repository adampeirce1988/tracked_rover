
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
    telemetryStaleAfterMs: 500
};

//=============================================================================*
// Internal State
//=============================================================================*

let vehicleSocket = null;               // Stores the websocket used for telemetry.
let connectionState = "disconnected";   // Tracks the connection state. connecting, connected or disconnected.
let lastMessageTime = null;             // stores a. timestamp for the last packet received.
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

        // Request the current vehicle state  
        requestTelemetryStatus();
        
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
            console.error("Invalid WebSocket JASON:", error);
            return; 
        }

        // Reject null arrays and non-object values before updating the telemetry 
        if(data === null || typeof data !== "object" || Array.isArray(data)){
            consle.error("invalid telemetry format: expecte a JSON object"); 
            return; 
        }

        lastMessageTime = Date.now();

        // Merge new fields without removing values from previous messages.
        Object.assign(latestValues, data);

        // notify the subscriber that the telemetry has changed
        notifyTelemetryListeners();

        // Log the updated telemetry store. 
        console.log("ESP-Nexus telemetry reveived:" ,latestValues);
    };

    // Log socket errors (reconnect is handled onclose). 
    vehicleSocket.onerror = function(error){
        console.error("ESP-Nexus websocket error: ",error); 
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


function requestTelemetryStatus(){

    // Request the current vehicle status if the socket is open.
    if(vehicleSocket !== null && vehicleSocket.readyState === WebSocket.OPEN){
            vehicleSocket.send("get_status");
        }
        else{
            console.warn("Cannot request ststus: WebSocket is not conected"); 
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

 function getTelemetryHealth(){

    const lastMessageAgeMs = lastMessageTime === null ? null : Date.now() - lastMessageTime; 

    return{
        connectionState: connectionState,
        hasReceivedTelemetry: lastMessageTime !== null, 
        lastMessageAgeMs: lastMessageAgeMs,
        telemetryStale: lastMessageAgeMs === null || lastMessageAgeMs > WS_CONFIG.telemetryStaleAfterMs
    };

 }

//=============================================================================*
// Public Interface
//=============================================================================*

const VehicleTelemetry = {

    connect: connectWebSocket, 
    testReconnect: testWebSocketReconnect,
    requestStatus: requestTelemetryStatus, 
    subscribe: subscribeTelemetry,
    unsubscribe:unsubscribeTelemetry,
    getConnectionState: getConectionStateInternal,
    getValue: getTelemetryValue,
    getAllValues: getAllTelemetryValues,
    getHealth: getTelemetryHealth,

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
 * The suite checks the public API, connection handling, telemetry reception,
 * data storage, subscribers, automatic reconnection and telemetry freshness.
 *
 * Results are logged to the console and returned as a summary object.
 *=============================================================================*/


async function runVehicleTelemetryTests() {

    const results = [];
    const timeoutMs = 4000;
    const staleWaitMs = WS_CONFIG.telemetryStaleAfterMs + 150;

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
            results.push({ Test: name, Result: "PASS" });
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

    // Request status and wait for the next telemetry update.
    async function requestAndWaitForMessage() {
        let unsubscribe;

        const messageReceived = new Promise((resolve, reject) => {
            const timeout = setTimeout(() => {
                if (unsubscribe) {
                    unsubscribe();
                }

                reject(new Error("No telemetry message received"));
            }, timeoutMs);

            unsubscribe = VehicleTelemetry.subscribe(() => {
                clearTimeout(timeout);
                unsubscribe();
                resolve();
            });
        });

        VehicleTelemetry.requestStatus();

        await messageReceived;
    }

    console.log("Starting VehicleTelemetry browser tests...");

    // 1. Check the public API.
    await runTest("Public API methods exist", async () => {
        const methods = [
            "connect",
            "testReconnect",
            "requestStatus",
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

    // 4. Confirm valid telemetry has arrived.
    await runTest("Telemetry is received", async () => {
        await waitFor(() =>
            VehicleTelemetry.getHealth().hasReceivedTelemetry
        );
    });

    // 5. Check telemetry is stored and readable.
    await runTest("Telemetry is stored", async () => {
        const values = VehicleTelemetry.getAllValues();

        if (
            values === null ||
            typeof values !== "object" ||
            Object.keys(values).length === 0
        ) {
            throw new Error("Telemetry store is empty");
        }
    });

    // 6. Check the health result while telemetry is fresh.
    await runTest("Health reports fresh telemetry", async () => {
        await requestAndWaitForMessage();

        const health = VehicleTelemetry.getHealth();

        if (health.connectionState !== "connected") {
            throw new Error("Connection is not reported as connected");
        }

        if (!health.hasReceivedTelemetry) {
            throw new Error("Telemetry has not been recorded");
        }

        if (health.lastMessageAgeMs === null) {
            throw new Error("Message age is missing");
        }

        if (health.telemetryStale) {
            throw new Error("Telemetry was reported as stale");
        }

        if (health.lastMessageAgeMs < 0) {
            throw new Error("Invalid message age");
        }
    });

    // 7. A subscriber must receive telemetry updates.
    await runTest("Subscriber receives updates", async () => {
        let received = false;

        const unsubscribe = VehicleTelemetry.subscribe(() => {
            received = true;
        });

        try {
            VehicleTelemetry.requestStatus();
            await waitFor(() => received);
        }
        finally {
            unsubscribe();
        }
    });

    // 8. Unsubscribed listeners must stop receiving updates.
    await runTest("Unsubscribe stops notifications", async () => {
        let count = 0;

        const unsubscribe = VehicleTelemetry.subscribe(() => {
            count++;
        });

        unsubscribe();

        const countAfterUnsubscribe = count;

        await requestAndWaitForMessage();
        await new Promise(resolve => setTimeout(resolve, 100));

        if (count !== countAfterUnsubscribe) {
            throw new Error("Subscriber received an update after unsubscribe");
        }
    });

    // 9. An invalid subscriber must not be registered.
    await runTest("Invalid subscriber is handled", async () => {
        const unsubscribe = VehicleTelemetry.subscribe(null);

        if (typeof unsubscribe !== "function") {
            throw new Error("Invalid subscriber did not return a cleanup function");
        }

        unsubscribe();
    });

    // 10. Closing the socket should update connection state.
    await runTest("Socket closes correctly", async () => {
        VehicleTelemetry.testReconnect();

        await waitFor(() =>
            VehicleTelemetry.getConnectionState() === "disconnected"
        );
    });

    // 11. With the socket closed, telemetry should become stale.
    await runTest("Telemetry becomes stale", async () => {
        await new Promise(resolve => setTimeout(resolve, staleWaitMs));

        const health = VehicleTelemetry.getHealth();

        if (health.connectionState !== "disconnected") {
            throw new Error("Socket reconnected before stale check");
        }

        if (!health.hasReceivedTelemetry) {
            throw new Error("Previous telemetry state was lost");
        }

        if (!health.telemetryStale) {
            throw new Error("Telemetry was not marked stale");
        }

        if (health.lastMessageAgeMs <= WS_CONFIG.telemetryStaleAfterMs) {
            throw new Error("Message age did not exceed the stale threshold");
        }
    });

    // 12. The socket should reconnect automatically.
    await runTest("Automatic reconnection works", async () => {
        await waitFor(() =>
            VehicleTelemetry.getConnectionState() === "connected",
            timeoutMs + WS_CONFIG.reconnectDelayMs + 2000
        );
    });

    // 13. A new message should restore fresh telemetry.
    await runTest("Telemetry recovers after reconnection", async () => {
        await requestAndWaitForMessage();

        const health = VehicleTelemetry.getHealth();

        if (health.connectionState !== "connected") {
            throw new Error("Connection is not healthy after reconnection");
        }

        if (!health.hasReceivedTelemetry) {
            throw new Error("Telemetry was not recorded after reconnection");
        }

        if (health.telemetryStale) {
            throw new Error("Telemetry remained stale after a new message");
        }

        if (health.lastMessageAgeMs > WS_CONFIG.telemetryStaleAfterMs) {
            throw new Error("New telemetry exceeded the stale threshold");
        }
    });

    // Display the final results.
    const passed = results.filter(result => result.Result === "PASS").length;
    const failed = results.length - passed;

    console.log(`RESULT: ${passed}/${results.length} passed`);

    if (failed > 0) {
        console.error("Failed tests:", results.filter(
            result => result.Result === "FAIL"
        ));
    }

    console.table(results);

    return {
        passed: passed,
        failed: failed,
        total: results.length,
        results: results
    };
}
