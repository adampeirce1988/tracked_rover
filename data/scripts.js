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
    // Connect HTML Elements
    //=============================================================================*

    const vehicleFSMStatus = document.getElementById("vehicle-state");

    // Verify HTML elements are available.
    console.log(vehicleFSMStatus);


    //=============================================================================*
    // Establish WebSocket Connection
    //=============================================================================*

    const vehicleSocket = new WebSocket(
        "ws://" + window.location.hostname + ":81/ws"
    );

    window.testSocket = vehicleSocket;


    // Handle a successful connection.
    vehicleSocket.onopen = function() {

        console.log("WebSocket connected");

        // Request the current vehicle status.
        vehicleSocket.send("get_status");
    };


    // Handle incoming status messages.
    vehicleSocket.onmessage = function(event) {

        console.log("WebSocket message:", event.data);

        try {

            const data = JSON.parse(event.data);

            if (data.type === "status") {
                vehicleFSMStatus.textContent = data.vehicle_state;
            }

        } catch (error) {

            console.error("Invalid WebSocket JSON:", error);
        }
    };


    // Report connection errors.
    vehicleSocket.onerror = function(error) {

        console.error("WebSocket error:", error);
    };


    // Report a closed connection.
    vehicleSocket.onclose = function() {

        console.log("WebSocket disconnected");
    };
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

        console.log("main menue: ", isOpen ? "Open" : "Closed");
    });


    // Complete initialisation.
    console.log("Main menu initialised");

    menuButton.setAttribute("aria-expanded", "false");
}


/*=============================================================================*
 * Vehicle Status Menu
 *=============================================================================*/

// Initialise the vehicle status menu.
function initStatussMenu(container) {

    // Connect HTML elements.
    const statusButton = container.querySelector(".vehicle-status-button");
    const statusDropdown = container.querySelector(".vehicle-status-dropdown");


    // Validate required HTML elements.
    if (!statusButton || !statusDropdown) {

        console.log("Unable to fund statusButton or statusDropdown");
        return;
    }


    // Handle vehicle status menu button clicks.
    statusButton.addEventListener("click", function() {

        const isOpen = statusDropdown.style.display === "block";

        statusDropdown.style.display = isOpen ? "none" : "block";
        statusButton.setAttribute("aria-expanded", String(!isOpen));

        console.log("vehiceStatus menu: ", isOpen ? "closed" : "Open");
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
                initStatussMenu(mainMenuContainer);
                updatePageTitle(); 

                console.log("Main menu loaded");
            })
            .catch(error => {

                console.log("Main menu loading error.", error);
            });
    }
}


