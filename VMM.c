#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PAGE_SIZE 1024   // Default page size in bytes
#define DEFAULT_NUM_PAGES 8      // Default number of virtual pages
#define DEFAULT_NUM_FRAMES 4     // Default number of physical frames

// Structure to represent a page table entry
typedef struct {
    int frame_number;
    int valid;  // Indicates if the page is in memory (1) or not (0)
} PageTableEntry;

// Structure for a page table
typedef struct {
    PageTableEntry *pages;
    int num_pages;
} PageTable;

// Structure to simulate physical memory (using a frame table)
typedef struct {
    int *page_loaded;    // Which virtual page is currently loaded into each frame
    int next_free_frame; // The next frame to be used in the FIFO page replacement
    int num_frames;
} FrameTable;

// Page table for the process
PageTable page_table;

// Frame table for the memory
FrameTable frame_table;

int page_size; // Customizable page size

// Initialize the page table and frame table
void initialize_memory_manager(int num_pages, int num_frames, int page_size_input) {
    page_size = page_size_input;

    // Initialize the page table
    page_table.pages = (PageTableEntry *)malloc(num_pages * sizeof(PageTableEntry));
    page_table.num_pages = num_pages;
    for (int i = 0; i < num_pages; i++) {
        page_table.pages[i].frame_number = -1; // Initially, no pages are loaded
        page_table.pages[i].valid = 0;         // Pages are not valid (i.e., not in memory)
    }

    // Initialize the frame table
    frame_table.page_loaded = (int *)malloc(num_frames * sizeof(int));
    frame_table.num_frames = num_frames;
    frame_table.next_free_frame = 0;
    for (int i = 0; i < num_frames; i++) {
        frame_table.page_loaded[i] = -1;       // No page is loaded in any frame
    }
}

// Simulate a page fault handler (FIFO page replacement)
void handle_page_fault(int virtual_page_number) {
    printf("Page fault occurred for page %d\n", virtual_page_number);

    // Get the frame that will be used for page replacement (FIFO strategy)
    int frame_to_replace = frame_table.next_free_frame;
    int page_to_replace = frame_table.page_loaded[frame_to_replace];

    // If there is a page to replace, mark it as invalid
    if (page_to_replace != -1) {
        page_table.pages[page_to_replace].valid = 0;
        printf("Replacing page %d in frame %d\n", page_to_replace, frame_to_replace);
    }

    // Load the new page into the selected frame
    page_table.pages[virtual_page_number].frame_number = frame_to_replace;
    page_table.pages[virtual_page_number].valid = 1;
    frame_table.page_loaded[frame_to_replace] = virtual_page_number;

    // Update the next frame to be replaced (FIFO order)
    frame_table.next_free_frame = (frame_table.next_free_frame + 1) % frame_table.num_frames;
}

// Translate a virtual address to a physical address
int translate_virtual_to_physical(int virtual_address) {
    int virtual_page_number = virtual_address / page_size;
    int offset = virtual_address % page_size;

    if (virtual_page_number >= page_table.num_pages) {
        printf("Invalid virtual address: %d\n", virtual_address);
        exit(1);
    }

    // Check if the page is in memory
    if (page_table.pages[virtual_page_number].valid == 0) {
        // Page fault: the page is not in memory
        handle_page_fault(virtual_page_number);
    }

    // Translate to physical address
    int frame_number = page_table.pages[virtual_page_number].frame_number;
    int physical_address = frame_number * page_size + offset;
    return physical_address;
}

// Simulate accessing a memory address
void access_memory(int virtual_address) {
    printf("Accessing virtual address: %d\n", virtual_address);
    int physical_address = translate_virtual_to_physical(virtual_address);
    printf("Translated to physical address: %d\n", physical_address);
}

// Main function to drive the program with user-defined input
int main() {
    int num_pages, num_frames, page_size_input;

    // Take user input for memory configuration
    printf("Enter the number of virtual pages: ");
    scanf("%d", &num_pages);

    printf("Enter the number of physical frames: ");
    scanf("%d", &num_frames);

    printf("Enter the page size (in bytes): ");
    scanf("%d", &page_size_input);

    // Initialize the memory manager (page table and frame table)
    initialize_memory_manager(num_pages, num_frames, page_size_input);

    // Simulate memory accesses with user input
    char choice;
    int virtual_address;
    do {
        printf("\nEnter virtual address to access (0 to quit): ");
        scanf("%d", &virtual_address);

        if (virtual_address == 0) {
            break;
        }

        access_memory(virtual_address);

        printf("\nDo you want to access another address? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    // Free dynamically allocated memory
    free(page_table.pages);
    free(frame_table.page_loaded);

    return 0;
}
