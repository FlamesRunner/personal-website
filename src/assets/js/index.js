// JS for index.html
"use strict";
(function() {
    const objects = [];

    let line_length = 200;
    let line_width = 2;
    let dx = 2;
    let x = 0; // Temporary animation variables

    const get_canvas = () => {
        return document.getElementById("rt-canvas");
    }

    const reset_x = () => {
        x = -line_length * 2;
    }

    const clear_background = (ctx, width, height) => {
        ctx.fillStyle = "#000000";
        ctx.fillRect(0, 0, width, height);
    }
    
    function init_rt() {
        console.log("Initializing 2D RT");

        // Get canvas
        var canvas = get_canvas();
        var ctx = canvas.getContext("2d");

        // Force resize to fit parent element width without exceeding bounds
        canvas.width = canvas.parentElement.clientWidth;

        // Set height to 1/2 width
        canvas.height = canvas.width / 2;

        // Draw a rectangle
        clear_background(ctx, canvas.width, canvas.height);
        
        // Reset X
        reset_x();
    }

    function trace() {
        var canvas = get_canvas();
        var ctx = canvas.getContext("2d");

        // Clear background
        clear_background(ctx, canvas.width, canvas.height);

        // For now, draw some text saying "Coming soon!" in the middle of the canvas
        ctx.font = "30px Arial";
        ctx.fillStyle = "#FFFFFF";
        ctx.textAlign = "center";
        ctx.fillText("Coming soon!", canvas.width / 2, canvas.height / 2);

        // Draw a rectangle with line_length and line_width, moving from left to right
        // and once it reaches the right side of the canvas, reset it to the left
        ctx.fillStyle = "#FFFFFF";
        ctx.fillRect(x, canvas.height - line_width, line_length, line_width);
        x += dx
        if (x > canvas.width + line_length) {
            reset_x();
        }

        // Request animation frame
        window.requestAnimationFrame(trace);
    }

    const init_gallery = () => {
        // Find all elements with the class gallery-item
        const gallery_items = document.getElementsByClassName("gallery-item");

        // Loop through all gallery items
        for (let i = 0; i < gallery_items.length; i++) {
            // Get the img element embedded in the gallery item
            const img = gallery_items[i].getElementsByTagName("img")[0];

            // Get the src attribute of the img element
            const src = img.getAttribute("data-src");

            // Set the on-click event of the gallery item to open the image in a new tab
            gallery_items[i].onclick = () => {
                window.open(src, "_blank");
            }
        }
    }

    const contact_form_handler = async (event) => {
        // Prevent default form submission
        event.preventDefault();

        // Get form data
        const form = document.getElementById("contact-form");
        const data = new FormData(form);

        // Send JSON request to /api/submit
        const response = await fetch("/api/submit", {
            method: "POST",
            body: JSON.stringify(Object.fromEntries(data)),
            headers: {
                "Content-Type": "application/json"
            }
        });

        // Get response JSON
        const json = await response.json();
        if (json.success) {
            // If successful, hide the form
            form.style.display = "none";
        }

        // Display response message
        const message = document.getElementById("contact-form-status");
        message.innerHTML = json.message;
     
        // Show message
        message.style.display = "block";
        
        form.reset();
    }

    window.onload = function() {
        init_rt();
        init_gallery();

        // Add event listener to contact form
        const form = document.getElementById("contact-form");
        form.addEventListener("submit", contact_form_handler);

        // Update copyright year
        const year = document.getElementById("copyright-year");
        year.innerHTML = new Date().getFullYear();

        // Start interval
        window.requestAnimationFrame(trace);
    }

    window.onresize = function() {
        init_rt();
    }
})();
