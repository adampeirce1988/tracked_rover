
document.addEventListener("DOMContentLoaded", function(){

    // Debug: verify scripts.js is avaliable
    console.log("scripts.js loaded"); 

   
    

    //=============================================================================*
    // Connect HTML elements
    //=============================================================================*

    const vehicleFSMStatus = document.getElementById("vehicle-state");

    // Debug: verify HTML elements are available
    console.log(vehicleFSMStatus); 

    //=============================================================================*
    // vehicel status menue
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


 
    // //=============================================================================*
    // // Firmware upload
    // //=============================================================================*
    // firmwareUploadButton.addEventListener("click", function(){

    //     const file = firmwareFile.files[0];

    //     // check a file exsists 
    //     if(!file){
    //         firmwareInfo.textContent ="No Firmware file selcted"; 
    //         console.log("No firmware file sellected");
    //         return; 
    //     }

    //     // Debug: verify firmware file information
    //     console.log("Starting firmware upload");
    //     console.log(file.name);
    //     console.log(file.size);

    //     // Create upload form
    //     const formData = new FormData(); 
    //     formData.append("firmware", file);

    //     // Debug verifys that the file size is stored correctly  
    //     console.log("Update URL:", `/update-firmware?size=${file.size}`);

    //     // Create HTTP request
    //     const xhr = new XMLHttpRequest();
    //     xhr.open("POST", `/update-firmware?size=${file.size}`, true);

    
    //     // Handle server response
    //     xhr.onload = function(){
    //         console.log("Upload response: ", xhr.status);
    //         console.log("Server response: ", xhr.responseText); 
    //     };

    //     // Handle upload failure
    //     xhr.onerror = function(){
    //         console.log("ERROR: upload filed"); 
    //     };

    //     // Send firmware files
    //     xhr.send(formData);

    // });

});



// function for loading the main meue into the place holder
function loadMainMenu(){

        const mainMenuContainer = document.getElementById("main-menu-container"); 

        if (!mainMenuContainer) {
            console.log("Main menu container not found on this page");
            return;
        }

         if(mainMenuContainer){
        fetch("/components/main_menu.html")
            .then(response => {
                if (!response.ok) {
                    throw new Error("Failed to load main menu: " + response.status);
                }

                return response.text();
            })
            .then( html => {
                mainMenuContainer.innerHTML = html;
                initialiseMainMenu(mainMenuContainer); 
                console.log("Main menu loaded");

            })
            .catch(error => {
                console.log("Main menu loading error.", error); 

            })
        }

    }

    // open expand and close the main menu 

    function initialiseMainMenu(container){

        const menuButton = container.querySelector(".main-menu-button");
        const menuContent = container.querySelector(".main-menu-content");

        if(!menuButton || !menuContent){
            console.error("Unable to find menuBtton or menuContent"); 
            return; 
        }

        menuButton.addEventListener("click", function(){
            const isOpen = menuContent.style.display === "block";
            menuContent.style.display = isOpen ? "none" : "block";
            menuButton.setAttribute("aria-expanded", String(!isOpen)); 

            console.log("main menue: ", isOpen ? "Open" : "Closed"); 
        });

        console.log("Main menu initialised");

        menuButton.setAttribute("aria-expanded", "false");

    }

   
   // load the vehicle status menu
    function loadVehicleMenu(){
        const vehicleMenuContainer = document.getElementById("vehicle-status-container");

        if(!vehicleMenuContainer){
            console.log("Vehicle menu container not found on this page");
            return; 
        }

        fetch("/components/vehicle_menu.html")
            .then(response => {
                if (!response.ok){
                    throw new Error("Failed to laod vehicle menu: " + response.status);
                }

                return response.text(); 
            })
            .then(html => {
                vehicleMenuContainer.innerHTML = html;
                console.log("vehicle menu loaded ");
            })
            .catch(error => {
                console.error("Vehicle menu loading error: ", error); 
            });
    }




