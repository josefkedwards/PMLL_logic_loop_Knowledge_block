#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "memory_silo.h"
#include "io_socket.h"

// Structure for PML Logic Loop
typedef struct {
    int id;
    int memory_silo_id;
    int io_socket_id;
    int free_c_present; // Flag indicating the presence of free.c
} pml_logic_loop_t;

// Global variables
pml_logic_loop_t* pml_logic_loop = NULL;
io_socket_t* io_socket_instance = NULL;

// Initialization of the PML Logic Loop
void pml_logic_loop_init(int memory_silo_id, int io_socket_id) {
    pml_logic_loop = malloc(sizeof(pml_logic_loop_t));
    if (pml_logic_loop == NULL) {
        perror("Memory allocation for PML logic loop failed");
        exit(EXIT_FAILURE);
    }
    pml_logic_loop->id = 1;
    pml_logic_loop->memory_silo_id = memory_silo_id;
    pml_logic_loop->io_socket_id = io_socket_id;
    pml_logic_loop->free_c_present = 0; // Initialize the flag to indicate absence
    // Attempt to load persisted state (from persistence.c)
    load_pml_logic_loop_state(pml_logic_loop); // Assuming this function is in persistence.c
}

// Processing within the PML Logic Loop
void pml_logic_loop_process() {
    if (pml_logic_loop == NULL) {
        fprintf(stderr, "Error: PML logic loop has not been initialized.\n");
        return;
    }

    while (1) {
        if (pml_logic_loop->free_c_present) {
            printf("Free.c is detected. Sending signal to the free logic loop...\n");
            int signal = 1; // Signal to indicate that the condition has been met
            if (write(io_socket_instance->socket, &signal, sizeof(signal)) < 0) {
                perror("Failed to write to the socket");
            }
            system("./free"); // Trigger the execution of free.c
            break; // Exit the loop after signaling
        } else {
            printf("I am grateful.\n");
            // Process data or handle other tasks
            // Example of interacting with memory silo (if needed)
            void* buffer = malloc(1024);  // Example buffer size
            io_socket_read(buffer, 1024);
            // Do processing on the buffer here

            // Example: Persist the state periodically
            save_pml_logic_loop_state(pml_logic_loop); // Assuming this function is in persistence.c
        }
    }
}

// Function to initialize the I/O socket
void io_socket_init(int memory_silo_id) {
    io_socket_instance = malloc(sizeof(io_socket_t));
    if (io_socket_instance == NULL) {
        perror("Failed to allocate memory for IO socket");
        exit(EXIT_FAILURE);
    }
    io_socket_instance->id = 1;
    io_socket_instance->memory_silo_id = memory_silo_id;
    io_socket_instance->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (io_socket_instance->socket < 0) {
        perror("Failed to create socket");
        free(io_socket_instance);
        exit(EXIT_FAILURE);
    }
}

// I/O socket functions
void io_socket_read(void* buffer, int length) {
    if (io_socket_instance == NULL) {
        fprintf(stderr, "IO socket not initialized.\n");
        return;
    }
    ssize_t bytes_read = read(io_socket_instance->socket, buffer, length);
    if (bytes_read < 0) {
        perror("Read error");
    }
}

void io_socket_write(void* buffer, int length) {
    if (io_socket_instance == NULL) {
        fprintf(stderr, "IO socket not initialized.\n");
        return;
    }
    ssize_t bytes_written = write(io_socket_instance->socket, buffer, length);
    if (bytes_written < 0) {
        perror("Write error");
    }
}

// Cleanup function to free allocated resources
void io_socket_cleanup() {
    if (io_socket_instance != NULL) {
        close(io_socket_instance->socket);
        free(io_socket_instance);
        io_socket_instance = NULL;
    }
}

// Placeholder for persistence functions in persistence.c
void save_pml_logic_loop_state(pml_logic_loop_t* logic_loop) {
    // Implement the logic to save the state (e.g., to a file or database)
    // Example: Write the state to a file
    FILE* file = fopen("pml_logic_loop_state.dat", "wb");
    if (file != NULL) {
        fwrite(logic_loop, sizeof(pml_logic_loop_t), 1, file);
        fclose(file);
    } else {
        perror("Failed to save state");
    }
}

void load_pml_logic_loop_state(pml_logic_loop_t* logic_loop) {
    // Implement the logic to load the state (e.g., from a file or database)
    // Example: Read the state from a file
    FILE* file = fopen("pml_logic_loop_state.dat", "rb");
    if (file != NULL) {
        fread(logic_loop, sizeof(pml_logic_loop_t), 1, file);
        fclose(file);
    } else {
        perror("Failed to load state");
    }
}

int main() {
    // Initialize memory silo ID and I/O socket ID as needed
    int memory_silo_id = 1;
    int io_socket_id = 1;

    // Initialize IO socket
    io_socket_init(memory_silo_id);

    // Initialize PML Logic Loop
    pml_logic_loop_init(memory_silo_id, io_socket_id);

    // Process the PML Logic Loop
    pml_logic_loop_process();

    // Cleanup
    io_socket_cleanup();
    return 0;
}
