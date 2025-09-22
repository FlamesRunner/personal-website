"use strict";
(function () {
    /* Settings */
    const DEBUG_MODE = false;

    /* Helper functions */

    /**
     * Asserts a boolean condition. Throws the msg as an error if the assertion fails.
     * 
     * @param {boolean} condition Boolean condition
     * @param {string} msg String msg
     * @returns {void}
     */
    const assert = (condition, msg = "Error not defined") => {
        if (!condition) throw new Error(msg);
    };

    /**
     * Logs a message, only if DEBUG_MODE is true.
     * 
     * @param {string} msg 
     * @returns {void}
     */
    const debugLog = (msg) => {
        if (DEBUG_MODE) console.log(msg);
    }

    /**
     * Gallery item class
     */
    class GalleryItem {
        galleryItemElement; // Refers to the element with class .gallery-item
        src;
        dataSrc;
        alt;
        caption;

        /**
         * Gallery Item Constructor
         * 
         * @param {HTMLElement} galleryItemElement 
         */
        constructor(galleryItemElement) {
            assert(galleryItemElement instanceof HTMLElement, "Cannot initialize Gallery Item; element in constructor must be of type HTMLElement");
            this.galleryItemElement = galleryItemElement;

            // Initialize other properties
            const galleryItemImgs = galleryItemElement.querySelectorAll("img");
            assert(galleryItemImgs.length === 1, "Gallery item must contain a single image element as a child");

            // Fill src
            const galleryImg = galleryItemImgs[0];
            assert(galleryImg.getAttribute("src") !== null, "Gallery item image must have src set (thumbnail)");
            this.src = galleryImg.getAttribute("src");

            // Fill data-src
            assert(galleryImg.getAttribute("data-src") !== null, "Gallery item image must have data-src set (high-quality image)");
            this.dataSrc = galleryImg.getAttribute("data-src");

            // Fill alt
            assert(galleryImg.getAttribute("alt") !== null, "Gallery item image must have alt text set (accessible desc.)");
            this.alt = galleryImg.getAttribute("alt");

            // Fill image caption
            const galleryCaptions = galleryItemElement.querySelectorAll("p");
            assert(galleryCaptions.length === 1, "Gallery item must contain a single gallery caption element (p) as a child");
            const galleryCaption = galleryCaptions[0];
            assert(galleryCaption.innerText !== "", "Gallery item caption must be set");
            this.caption = galleryCaption.innerText;
        }
    }

    /**
     * Gallery modal class
     */
    class GalleryModal {
        /** @type {HTMLElement} Gallery modal element reference */
        galleryModalElement;

        /** @type {GalleryItem[]} Gallery item list */
        galleryItems = [];

        /** @type {"CLOSED" | "OPEN"} Gallery modal status */
        state = "CLOSED";

        /** @type {HTMLElement} Gallery modal close button */
        closeModalBtnElement;

        /** @type {EventListener | null} Gallery modal close button event listener */
        closeModalBtnElementEventListener = () => {
            this.closeModal();
        };

        /** @type {HTMLImageElement} Gallery modal image element */
        galleryModalImageElement;

        /** @type {HTMLElement} Gallery modal image caption element */
        galleryModalImageCaptionElement;

        /** @type {HTMLElement} Gallery modal image background filler element */
        galleryModalImageBgFillerElement;

        /** @type {HTMLImageElement} Gallery modal image loader image */
        galleryModalImageLoader = new Image();

        /** @type {HTMLElement} Gallery modal image loader element */
        galleryModalImageLoaderElement;

        /**
         * Gallery Modal Constructor
         * 
         * @param {HTMLElement} galleryModalElement Gallery modal element
         */
        constructor(galleryModalElement) {
            assert(galleryModalElement instanceof HTMLElement, "Cannot initialize Gallery Modal; element in constructor must be of type HTMLElement");
            this.galleryModalElement = galleryModalElement;

            const closeModalBtnElements = galleryModalElement.getElementsByClassName("gallery-modal-close");
            assert(closeModalBtnElements.length === 1, "Gallery modal must have exactly one close button");
            this.closeModalBtnElement = closeModalBtnElements[0];
            this.closeModalBtnElement.addEventListener("click", this.closeModalBtnElementEventListener);

            const imageElements = galleryModalElement.getElementsByClassName("gallery-modal-image");
            assert(imageElements.length === 1, "Gallery modal must contain exactly one gallery-modal type image element");
            this.galleryModalImageElement = imageElements[0];

            const imageCaptionElements = galleryModalElement.getElementsByClassName("gallery-modal-caption");
            assert(imageCaptionElements.length === 1, "Gallery modal must contain exactly one gallery-modal type caption element");
            this.galleryModalImageCaptionElement = imageCaptionElements[0];

            const imageBgFillerElements = galleryModalElement.getElementsByClassName("gallery-modal-image-bg");
            assert(imageBgFillerElements.length === 1, "Gallery modal must contain exactly one BG-filler type element");
            this.galleryModalImageBgFillerElement = imageBgFillerElements[0];

            const imageLoaderElements = galleryModalElement.getElementsByClassName("gallery-modal-image-loader");
            assert(imageLoaderElements.length === 1, "Gallery modal must contain exactly one image loader type element");
            this.galleryModalImageLoaderElement = imageLoaderElements[0];

            this.galleryModalImageLoader.src = "/assets/media/loading.gif";
            this.galleryModalImageLoader.loading = "eager";
            this.galleryModalImageLoaderElement.src = this.galleryModalImageLoader.src;
        }

        /**
         * Closes the gallery modal.
         * 
         * @returns {void}
         */
        closeModal() {
            assert(this.state === "OPEN", "Modal must be open before it can be closed");

            this.galleryModalElement.style.opacity = 0;

            setTimeout(() => {
                this.state = "CLOSED";
                this.galleryModalImageElement.setAttribute("src", "");
                this.galleryModalImageElement.setAttribute("alt", "");
                this.galleryModalImageCaptionElement.innerHTML = "";
                this.galleryModalImageBgFillerElement.style.backgroundImage = "";
                this.galleryModalImageLoaderElement.style.opacity = 0;
                this.galleryModalElement.classList.remove("gallery-modal-opened");
                this.galleryModalElement.classList.add("gallery-modal-closed");
                document.body.style.removeProperty("overflow-y");
            }, 500);
        }

        /**
         * Opens the modal.
         * 
         * @param {GalleryItem} galleryItem Gallery item instance
         * @returns {void}
         */
        openModal(galleryItem) {
            assert(galleryItem instanceof GalleryItem, "Parameter must be of type GalleryItem");
            assert(this.state === "CLOSED", "Modal must be closed before it can be opened");

            this.galleryModalImageElement.style.opacity = 0;
            this.galleryModalImageLoaderElement.style.opacity = 1;
            this.galleryModalImageElement.setAttribute("src", galleryItem.dataSrc);

            const intval = setInterval(() => {
                if (this.galleryModalImageElement.complete) {
                    this.galleryModalImageElement.style.opacity = 1;
                    this.galleryModalImageLoaderElement.style.opacity = 0;
                    clearInterval(intval);
                    this.galleryModalImageElement.onprogress = null;
                }
            }, 1000);

            this.galleryModalImageElement.setAttribute("alt", galleryItem.alt);
            this.galleryModalImageCaptionElement.innerHTML = galleryItem.caption;
            this.galleryModalImageBgFillerElement.style.backgroundImage = `url(${galleryItem.src})`;

            this.galleryModalElement.classList.remove("gallery-modal-closed");
            this.galleryModalElement.classList.add("gallery-modal-opened");

            setTimeout(() => {
                this.galleryModalElement.style.opacity = 1;
            }, 100);

            this.state = "OPEN";
            document.body.style.overflowY = "hidden";
        }

        /**
         * Register item click listener for a gallery item tied to this modal.
         * 
         * @param {GalleryItem} galleryItem Gallery item instance
         * @returns {void}
         */
        registerItemClickListener(galleryItem) {
            assert(galleryItem instanceof GalleryItem, "Parameter must be of type GalleryItem");
            galleryItem.galleryItemElement.addEventListener("click", () => {
                this.openModal(galleryItem);
            });
        }

        /**
         * Insert gallery item into the tracking list.
         * 
         * @param {GalleryItem} galleryItem Gallery item to track
         * @returns {void}
         */
        insertGalleryItem(galleryItem) {
            assert(galleryItem instanceof GalleryItem, "Parameter must be of type GalleryItem");
            this.galleryItems.push(galleryItem);
            this.registerItemClickListener(galleryItem);
            debugLog(`Registered gallery item with src=${galleryItem.src}`);
        }
    }

    /**
     * Initializes the gallery
     * 
     * @param {GalleryModal} galleryModal Instance of GalleryModal
     */
    const init_gallery = (galleryModal) => {
        assert(galleryModal instanceof GalleryModal, "galleryModal must be an instance of GalleryModal and must not be null");

        /** @type {HTMLCollection} */
        const gallerySections = document.getElementsByClassName("gallery");
        assert(gallerySections.length === 1, "Must have exactly one gallery class delcared");

        /** @type {HTMLElement} */
        const gallerySection = gallerySections[0];

        /* Discover gallery items */
        /** @type {HTMLCollection} */
        const galleryItems = gallerySection.querySelectorAll(".gallery-item");
        assert(galleryItems.length > 0, "Must have at least one gallery item");
        debugLog(`Discovered ${galleryItems.length} gallery items`);

        /* Register gallery items */
        for (let i = 0; i < galleryItems.length; i++) {
            const galleryItemEl = galleryItems.item(i);
            const galleryItem = new GalleryItem(galleryItemEl);
            console.log(galleryItem);
            galleryModal.insertGalleryItem(galleryItem);
        }
    };

    window.addEventListener("load", () => {
        const galleryModalElement = document.getElementById("gallery-modal");
        const galleryModal = new GalleryModal(galleryModalElement);
        init_gallery(galleryModal);
    });
})();