
"use strict";
(function () {
    window.addEventListener("onload", function () {
        // Add event listener to contact form
        const form = document.getElementById("contact-form");
        form.addEventListener("submit", contact_form_handler);

        // Update copyright year
        const year = document.getElementById("copyright-year");
        year.innerHTML = new Date().getFullYear();
    });
})();