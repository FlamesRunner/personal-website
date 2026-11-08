"use strict";
(function () {
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

    const toggleMobileMenu = (event) => {
        event.preventDefault();
        const nav = document.getElementById("nav-mobile-menu");
        nav.classList.toggle("nav-mobile-active");
    }

    const closeMobileMenu = (event) => {
        event.preventDefault();
        const nav = document.getElementById("nav-mobile-menu");
        nav.classList.remove("nav-mobile-active");

        // now navigate to the link
        const link = event.target;
        const href = link.getAttribute("href");
        window.location.href = href;
    }

    window.addEventListener("load", function () {
        // Add event listener to contact form
        const form = document.getElementById("contact-form");
        form.addEventListener("submit", contact_form_handler);

        // Update copyright year
        const year = document.getElementById("copyright-year");
        year.innerHTML = new Date().getFullYear();

        // Mobile nav
        const toggleMobileNavBtn = document.getElementById("toggleMobileNavBtn");
        toggleMobileNavBtn.addEventListener("click", (event) => {
            toggleMobileMenu(event);
        });

        // Mobile nav link handler
        const mobileLinks = document.querySelectorAll(".nav-mobile-link");
        mobileLinks.forEach((link) => {
            link.addEventListener("click", (event) => {
                closeMobileMenu(event);
            });
        });
    });

    window.addEventListener("scroll", function () {
        const sections = document.getElementsByClassName("nav-section");
        const scroll_position = window.scrollY;
        const sections_in_view = [];

        for (let i = 0; i < sections.length; i++) {
            const section = sections[i];
            const section_id = section.id;
            const section_top = section.offsetTop;
            const section_bottom = section_top + section.offsetHeight;

            if (scroll_position + 80 >= section_top && scroll_position <= section_bottom) {
                sections_in_view.push(section_id);
            }
        }

        // If more than one section is in view, choose the first one
        // Nav links corresponding to each section have data-section="id" set
        if (sections_in_view.length > 0) {
            // Remove active class from other nav links (elements with data-section set)
            const nav_links = document.querySelectorAll("a[data-section]");
            for (let i = 0; i < nav_links.length; i++) {
                nav_links[i].classList.remove("nav-link-active");
            }

            // Pick the section closest to the scroll position
            let section = sections_in_view[0];
            for (let i = 0; i < sections_in_view.length; i++) {
                if (Math.abs(document.getElementById(section).scrollHeight - scroll_position) > Math.abs(document.getElementById(sections_in_view[i]).scrollHeight - scroll_position)) {
                    section = sections_in_view[i];
                }
            }
            const query = `a[data-section="${section}"]`;
            const nav_link = document.querySelector(query);
            nav_link.classList.add("nav-link-active");
        }
    });
})();
