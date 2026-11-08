
"use strict";
(function () {
    window.addEventListener("load", function () {
        // Update copyright year
        const year = document.getElementById("copyright-year");
        year.innerHTML = new Date().getFullYear();
    });
})();