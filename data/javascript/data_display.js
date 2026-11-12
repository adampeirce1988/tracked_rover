document.addEventListener("DOMContentLoaded", function() {

    console.log("data_display.js loaded sucsesfuly.");

    // load the main menu
    loadMainMenu();

    // Initialise parameter selection
    initialiseParameterSelection();
  
});


/*=============================================================================*
 * Parameter Selection
 *=============================================================================*/

const MAX_SELECTED_PARAMETERS = 5;  // change to 10.  



function initialiseParameterSelection()
{
    const checkboxes = document.querySelectorAll(
        ".parameter-row input[type='checkbox']"
    );

    checkboxes.forEach(function(checkbox){

        checkbox.addEventListener("change", function(){

            const selectedCount = document.querySelectorAll(".parameter-row input[type='checkbox']:checked").length;

            if (selectedCount > MAX_SELECTED_PARAMETERS)
            {
                checkbox.checked = false;
                console.warn("Maximum of 10 parameters selected.");
            }

            updateParameterCount();
        });
    });

    updateParameterCount();
}


/*=============================================================================*
 * Update Parameter Count
 **=============================================================================*/

function updateParameterCount(){

    const selectedCount = document.querySelectorAll( ".parameter-row input[type='checkbox']:checked").length;

    const countDisplay = document.querySelector(".parameter-count");

    if(countDisplay){
        countDisplay.textContent = `${selectedCount} / ${MAX_SELECTED_PARAMETERS} selected`;
    }
}

document.addEventListener("DOMContentLoaded", function()
{
    initialiseParameterSelection();
});