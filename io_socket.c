#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// IO Socket Structure
typedef struct {
    int id;
    int memory_silo_id;
    int socket;
} io_socket_t;

// Global variable to hold the IO socket instance
io_socket_t* io_socket_instance = NULL;

// IO Socket Functions
void io_socket_init(int memory_silo_id) {
    // Initialize the IO socket
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

void io_socket_read(void* buffer, int length) {
    // Read from the IO socket
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
    // Write to the IO socket
    if (io_socket_instance == NULL) {
        fprintf(stderr, "IO socket not initialized.\n");
        return;
    }
    ssize_t bytes_written = write(io_socket_instance->socket, buffer, length);
    if (bytes_written < 0) {
        perror("Write error");
    }
}

// Get IO Socket Function (for demonstration, returns the global instance)
io_socket_t* get_io_socket(void) {
    // Get the IO socket associated with the socket
    return io_socket_instance;
}

// Cleanup function to free allocated resources
void io_socket_cleanup() {
    if (io_socket_instance != NULL) {
        close(io_socket_instance->socket);
        free(io_socket_instance);
        io_socket_instance = NULL;
    }
}
