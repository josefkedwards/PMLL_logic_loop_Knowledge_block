#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Memory Silo Structure
typedef struct {
    int id;
    int io_socket;
    void* memory;
    size_t size;
} memory_silo_t;

// Global memory silo list
memory_silo_t* memory_silos = NULL;
int memory_silo_count = 0;

// Function to Initialize Memory Silo
void memory_silo_init(int io_socket) {
    memory_silo_t* new_silo = malloc(sizeof(memory_silo_t));
    if (new_silo == NULL) {
        perror("Failed to allocate memory for memory silo");
        exit(EXIT_FAILURE);
    }

    // Initialize the silo
    new_silo->id = memory_silo_count + 1;
    new_silo->io_socket = io_socket;
    new_silo->size = 1024;  // Example size: 1KB
    new_silo->memory = malloc(new_silo->size);
    if (new_silo->memory == NULL) {
        perror("Failed to allocate memory for silo data");
        free(new_silo);
        exit(EXIT_FAILURE);
    }

    // Add to global list
    memory_silos = realloc(memory_silos, sizeof(memory_silo_t) * (memory_silo_count + 1));
    if (memory_silos == NULL) {
        perror("Failed to resize memory silo list");
        free(new_silo->memory);
        free(new_silo);
        exit(EXIT_FAILURE);
    }

    memory_silos[memory_silo_count] = *new_silo;
    memory_silo_count++;
    free(new_silo);  // Free the temporary allocation
}

// Function to Get Memory Silo
memory_silo_t* get_memory_silo(int io_socket) {
    for (int i = 0; i < memory_silo_count; i++) {
        if (memory_silos[i].io_socket == io_socket) {
            return &memory_silos[i];
        }
    }
    return NULL;
}

// Function to Write to Memory Silo
void memory_silo_write(int io_socket, void* buffer, size_t length) {
    memory_silo_t* silo = get_memory_silo(io_socket);
    if (silo == NULL) {
        fprintf(stderr, "Memory silo not found for IO socket %d\n", io_socket);
        return;
    }
    if (length > silo->size) {
        fprintf(stderr, "Error: Write length exceeds memory silo capacity\n");
        return;
    }
    memcpy(silo->memory, buffer, length);
}

// Function to Read from Memory Silo
void memory_silo_read(int io_socket, void* buffer, size_t length) {
    memory_silo_t* silo = get_memory_silo(io_socket);
    if (silo == NULL) {
        fprintf(stderr, "Memory silo not found for IO socket %d\n", io_socket);
        return;
    }
    if (length > silo->size) {
        fprintf(stderr, "Error: Read length exceeds memory silo capacity\n");
        return;
    }
    memcpy(buffer, silo->memory, length);
}

// Function to Cleanup Memory Silos
void cleanup_memory_silos() {
    for (int i = 0; i < memory_silo_count; i++) {
        free(memory_silos[i].memory);
    }
    free(memory_silos);
    memory_silos = NULL;
    memory_silo_count = 0;
}

// Main function for testing
int main() {
    int io_socket1 = 1;
    int io_socket2 = 2;

    memory_silo_init(io_socket1);
    memory_silo_init(io_socket2);

    char data[] = "Hello, Memory Silo!";
    memory_silo_write(io_socket1, data, sizeof(data));

    char buffer[1024];
    memory_silo_read(io_socket1, buffer, sizeof(data));
    printf("Read from Memory Silo 1: %s\n", buffer);

    cleanup_memory_silos();
    return 0;
}