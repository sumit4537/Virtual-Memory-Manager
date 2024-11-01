let pageTable = [];
let frameTable = [];
let pageSize, numPages, numFrames;
let nextFrameIndex = 0;

function initializeMemoryManager() {
    // Retrieve settings from user input
    numPages = parseInt(document.getElementById('num-pages').value);
    numFrames = parseInt(document.getElementById('num-frames').value);
    pageSize = parseInt(document.getElementById('page-size').value);

    // Initialize page table and frame table
    pageTable = Array.from({ length: numPages }, () => ({ frame: -1, valid: false }));
    frameTable = Array.from({ length: numFrames }, () => -1);

    nextFrameIndex = 0;
    document.getElementById("output-log").textContent = "Memory Manager Initialized.\n";
    logMessage(`Virtual Pages: ${numPages}, Physical Frames: ${numFrames}, Page Size: ${pageSize} bytes`);
}

function handlePageFault(pageNumber) {
    logMessage(`Page fault for page ${pageNumber}`);
    const frameToReplace = nextFrameIndex;
    const pageToReplace = frameTable[frameToReplace];

    // If there is a page in this frame, invalidate it
    if (pageToReplace !== -1) {
        pageTable[pageToReplace].valid = false;
        logMessage(`Replacing page ${pageToReplace} in frame ${frameToReplace}`);
    }

    // Load the new page into the frame
    pageTable[pageNumber].frame = frameToReplace;
    pageTable[pageNumber].valid = true;
    frameTable[frameToReplace] = pageNumber;

    // Update the next frame index (FIFO replacement)
    nextFrameIndex = (nextFrameIndex + 1) % numFrames;
}

function translateVirtualToPhysical(virtualAddress) {
    const pageNumber = Math.floor(virtualAddress / pageSize);
    const offset = virtualAddress % pageSize;

    if (pageNumber >= numPages) {
        logMessage(`Invalid virtual address: ${virtualAddress}`);
        return null;
    }

    if (!pageTable[pageNumber].valid) {
        handlePageFault(pageNumber);
    }

    const frameNumber = pageTable[pageNumber].frame;
    return frameNumber * pageSize + offset;
}

function accessMemory() {
    const virtualAddress = parseInt(document.getElementById('virtual-address').value);
    if (isNaN(virtualAddress)) {
        logMessage("Please enter a valid virtual address.");
        return;
    }

    logMessage(`Accessing virtual address: ${virtualAddress}`);
    const physicalAddress = translateVirtualToPhysical(virtualAddress);
    if (physicalAddress !== null) {
        logMessage(`Translated to physical address: ${physicalAddress}`);
    }
}

function logMessage(message) {
    const outputLog = document.getElementById("output-log");
    outputLog.textContent += message + "\n";
    outputLog.scrollTop = outputLog.scrollHeight; // Scroll to the bottom
}
