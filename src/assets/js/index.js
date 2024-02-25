// JS for index.html
"use strict";
(function () {
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

    const gallery_item_click = (event, galleryModalId) => {
        const gallery_item_click_helper = (event) => {
            // Get modal and modal image
            const modal = document.getElementById(galleryModalId);
            const modal_img = modal.getElementsByClassName("gallery-modal-image")[0];
            const modal_caption = modal.getElementsByClassName("gallery-modal-caption")[0];
            
            console.log(event);
            console.log(event.srcElement);

            let src, alt, caption;
            // Set caption to gallery-item-caption innerHTML
            if (event.srcElement.tagName === "IMG") {
                // If the event target is the img, set the src of the modal img to the src of the clicked img
                src = event.srcElement.getAttribute("data-src");
                alt = event.srcElement.alt;
                caption = event.srcElement.parentElement.getElementsByClassName("gallery-item-caption")[0].innerHTML;
            } else if (event.srcElement.tagName === "DIV") {
                // If the event target is the div, set the src of the modal img to the src of the img inside the div
                src = event.srcElement.getElementsByTagName("img")[0].getAttribute("data-src");
                alt = event.srcElement.getElementsByTagName("img")[0].alt;
                caption = event.srcElement.getElementsByClassName("gallery-item-caption")[0].innerHTML;
            } else if (event.srcElement.tagName === "P") {
                // If the event target is the p, set the src of the modal img to the src of the img inside the div
                src = event.srcElement.parentElement.getElementsByTagName("img")[0].getAttribute("data-src");
                alt = event.srcElement.parentElement.getElementsByTagName("img")[0].alt;
                caption = event.srcElement.innerHTML;
            }

            // Set the src of the modal img to the src of the clicked img
            modal_img.src = src;
            modal_img.alt = alt;

            // Set the caption of the modal to the alt of the clicked img
            modal_caption.innerHTML = caption;

            // Update gallery-modal-image-container to have background image
            const modal_img_container = modal.getElementsByClassName("gallery-modal-image-bg")[0];
            modal_img_container.style.backgroundImage = `url(${src})`;

            // Spool up the modal (remove hidden class)
            modal.classList.remove("gallery-modal-invisible");
            setTimeout(() => {
                modal.classList.remove("gallery-modal-hidden");
                modal.classList.add("gallery-modal");
            }, 100);
        }

        return gallery_item_click_helper(event);
    }

    const gallery_modal_init = (galleryModalId) => {
        const gallery_modal = document.getElementById(galleryModalId);
        const gallery_modal_close = gallery_modal.getElementsByClassName("gallery-modal-close")[0];

        gallery_modal_close.onclick = () => {
            gallery_modal.classList.add("gallery-modal-hidden");
            gallery_modal.classList.remove("gallery-modal");
            setTimeout(() => {
                gallery_modal.classList.add("gallery-modal-invisible");
            }, 200);
        }
    }

    const gallery_item_init = (galleryModalId) => {
        const gallery_items = document.getElementsByClassName("gallery-item");
        for (let i = 0; i < gallery_items.length; i++) {
            gallery_items[i].onclick = (event) => {
                gallery_item_click(event, galleryModalId);
            }
        }
    }

    window.onload = function () {
        // init_rt();
        init_gallery();
        gallery_modal_init("gallery-modal");
        gallery_item_init("gallery-modal");

        // Add event listener to contact form
        const form = document.getElementById("contact-form");
        form.addEventListener("submit", contact_form_handler);

        // Update copyright year
        const year = document.getElementById("copyright-year");
        year.innerHTML = new Date().getFullYear();

        // Start interval
        // window.requestAnimationFrame(trace);

        // Add window click event listener
        window.addEventListener("click", (event) => {
            // If the video with id rt-video is not playing, play it
            const video = document.getElementById("rt-video");
            if (video.paused) {
                video.play();
            }
        });
    }

    window.onresize = function () {
        // init_rt();
    }
})();
