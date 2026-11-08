/*=============================================================================*
 * ESP-Nexus - Client-Side Scripts
 *=============================================================================*/

/*=============================================================================*
 * WebSocket Connection & Vehicle Status
 *=============================================================================*/

document.addEventListener("DOMContentLoaded", function() {

    // Verify scripts.js is available.
    console.log("scripts.js loaded");

    //=============================================================================*
    // Establish WebSocket Connection
    //=============================================================================*
    
    // check the the scheduler module is avaliable 
    if(!window.UIScheduler){
        console.error("UIScheduler moduler is not avaliable"); 
        return; 
    }

    // Check that the shared Telemetry module is avaliable 
    if(!window.VehicleTelemetry){
        console.error("VehicleTelemetry module is not avaiable");
        return;
    }

    console.log("UIScheduler module available");
    console.log("VehicleTelemetry module available");

    // Subscribe to incoming telemetry updates.
    // Identify the channel from the message type and pass the data
    // to the corresponding display updater.
    window.VehicleTelemetry.subscribe(function(data) {

        switch(data.type) {

            case "ls_telemetry":
                updateLowSpeedTelemetry(data);
                break;

            case "hs_telemetry":
                updateHighSpeedTelemetry(data);
                break;

            default:
                console.warn("Unknown telemetry message type:", data.type);
                break;
        }
    });


    // Start the shared UI Scheduler 
    window.UIScheduler.start(); 

    // Conect to the telemetrey module 
    window.VehicleTelemetry.connect();


 });


/*=============================================================================*
 * Main Navigation Menu
 *=============================================================================*/

// Initialise the navigation menu.
function initMainMenu(container) {

    // Connect HTML elements.
    const menuButton = container.querySelector(".main-menu-button");
    const menuContent = container.querySelector(".main-menu-content");


    // Validate required HTML elements.
    if (!menuButton || !menuContent) {

        console.error("Unable to find menuBtton or menuContent");
        return;
    }


    // Handle navigation menu button clicks.
    menuButton.addEventListener("click", function() {

        const isOpen = menuContent.style.display === "block";

        menuContent.style.display = isOpen ? "none" : "block";
        menuButton.setAttribute("aria-expanded", String(!isOpen));

        console.log("main menue: ", isOpen ? "Closed" : "Open");

    });


    // Complete initialisation.
    console.log("Main menu initialised");

    menuButton.setAttribute("aria-expanded", "false");
}


/*=============================================================================*
 * Vehicle Status Menu
 *=============================================================================*/

// Initialise the vehicle status menu.
function initStatusMenu(container) {

    // Connect HTML elements.
    const statusButton = container.querySelector(".vehicle-status-button");
    const statusDropdown = container.querySelector(".vehicle-status-dropdown");


    // Validate required HTML elements.
    if (!statusButton || !statusDropdown) {

        console.log("Unable to find statusButton or statusDropdown");
        return;
    }


    // Handle vehicle status menu button clicks.
    statusButton.addEventListener("click", function() {

        const isOpen = statusDropdown.style.display === "block";

        statusDropdown.style.display = isOpen ? "none" : "block";
        statusButton.setAttribute("aria-expanded", String(!isOpen));

        console.log("vehice Status menu: ", isOpen ? "closed" : "Open");
    });
}



function updatePageTitle() {

    // Find the page name in the current HTML document.
    const pageName = document.querySelector(".page-name");

    // Validate the page name.
    if (!pageName) {
        console.warn("Page name not found");
        return;
    }

    const title = `ESP-Nexus - ${pageName.textContent.trim()}`;
    // Update the browser tab title.
    
    document.title = title;

    const appName = document.querySelector(".app-name");

    if(!appName){
        console.log("appName not found");
        return; 
    }

    appName.textContent = title; 

}

/*=============================================================================*
 * Main Menu Loader
 *=============================================================================*/

// Load the main menu into its placeholder.
function loadMainMenu() {

    const mainMenuContainer = document.getElementById("main-menu-container");


    // Validate the menu placeholder.
    if (!mainMenuContainer) {

        console.log("Main menu container not found on this page");
        return;
    }


    // Retrieve and initialise the main menu.
    if (mainMenuContainer) {

        fetch("/components/main_menu.html")
            .then(response => {

                if (!response.ok) {
                    throw new Error("Failed to load main menu: " + response.status);
                }

                return response.text();
            })
            .then(html => {

                mainMenuContainer.innerHTML = html;

                initMainMenu(mainMenuContainer);
                initStatusMenu(mainMenuContainer);
                updatePageTitle(); 

                console.log("Main menu loaded");
            })
            .catch(error => {

                console.log("Main menu loading error.", error);
            });
    }
}


/*=============================================================================*
 * Telemetry Display Updates
 *=============================================================================*/

// Update values received through the low-speed telemetry channel.
// This channel contains data that changes relatively slowly, such as
// vehicle status, battery voltage and Wi-Fi information.
function updateLowSpeedTelemetry(data) {

    // Update vehicle status.
    document.querySelectorAll(".vehicle-state-value").forEach(function(element) {
        element.textContent = data.vehicle_status ?? "--";
    });

    // Update battery voltage
    document.querySelectorAll(".vehicle-battery-value").forEach(function(element){
        element.textContent = data.battery_voltage != null        
        ? `${data.battery_voltage.toFixed(2)} v`: "--";
    }); 

    // Update WiFi opperation mode
    document.querySelectorAll(".wifi-mode-value").forEach(function(element) {
        element.textContent = data.wifi_mode ?? "--"; 
    });

    // Update wifi strength
    document.querySelectorAll(".wifi-signal-strength").forEach(function(element){
        element.textContent = data.wifi_signal != null
        ? `${data.wifi_signal} dBm` : "--";
    });


}


// Update values received through the high-speed telemetry channel.
// This channel contains rapidly changing sensor data, such as
// accelerometer and gyroscope readings.
function updateHighSpeedTelemetry(data) {

    // Update accelerometer readings.
    document.querySelectorAll(".acc-x-value").forEach(function(element) {
        element.textContent = data.acc_x ?? "--";
    });

    document.querySelectorAll(".acc-y-value").forEach(function(element) {
        element.textContent = data.acc_y ?? "--";
    });

    document.querySelectorAll(".acc-z-value").forEach(function(element) {
        element.textContent = data.acc_z ?? "--";
    });

    // Update gyroscope readings.
    document.querySelectorAll(".gyro-x-value").forEach(function(element) {
        element.textContent = data.gyro_x ?? "--";
    });

    document.querySelectorAll(".gyro-y-value").forEach(function(element) {
        element.textContent = data.gyro_y ?? "--";
    });

    document.querySelectorAll(".gyro-z-value").forEach(function(element) {
        element.textContent = data.gyro_z ?? "--";
    });
}