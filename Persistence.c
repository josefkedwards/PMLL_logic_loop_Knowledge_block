#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io_socket.h"
#include "pml_logic_loop.h"
#include "memory_silo.h"

// Global IO socket instance
io_socket_t io_socket;

// Function to send knowledge graph data (e.g., metrics)
void send_graph_node(io_socket_t *io_socket, const char *node_name, pml_metrics_t *metrics) {
    size_t node_name_length = strlen(node_name) + 1; // Include null terminator
    size_t metrics_size = sizeof(pml_metrics_t);
    size_t total_size = node_name_length + metrics_size;

    // Allocate memory for the data to be sent
    char *data = malloc(total_size);
    if (!data) {
        perror("Memory allocation failed");
        return;
    }

    // Copy the node name and metrics into the data buffer
    memcpy(data, node_name, node_name_length);
    memcpy(data + node_name_length, metrics, metrics_size);

    // Send the data through the socket
    if (io_socket_send(io_socket->socket, data, total_size) < 0) {
        fprintf(stderr, "Failed to send data\n");
    }

    free(data);
}

// Function to initialize the PML logic loop
void pml_logic_loop_init(io_socket_t *io_socket, int memory_silo_id) {
    pml_logic_loop_init(memory_silo_id, io_socket->socket);
}

// Function to handle the main PML logic loop with socket communication
void pml_logic_loop_process(io_socket_t *io_socket) {
    // Assuming we are using the knowledge graph and PML metrics
    pml_metrics_t metrics = {0.12, 0.85, 0.95}; // Example metrics
    const char *node_name = "Model A";

    // Send knowledge graph data through the socket
    send_graph_node(io_socket, node_name, &metrics);

    // Process the PML logic loop
    pml_logic_loop_process(io_socket->socket, NULL, 0);
}

// Function to initialize the IO socket
int io_socket_init_wrapper(io_socket_t *io_socket, const char *ip, int port) {
    return io_socket_init(io_socket, ip, port);
}

// Function to clean up and close the IO socket
void io_socket_cleanup(io_socket_t *io_socket) {
    io_socket_close(io_socket);
}

// Main function to demonstrate persistence with socket and PML logic loop
int main() {
    // Initialize the IO socket
    if (io_socket_init_wrapper(&io_socket, "127.0.0.1", 8080) < 0) {
        fprintf(stderr, "Failed to initialize IO socket\n");
        return -1;
    }

    // Initialize memory silo and PML logic loop
    int memory_silo_id = 1;  // Example silo ID
    pml_logic_loop_init(&io_socket, memory_silo_id);

    // Main loop for processing
    while (1) {
        pml_logic_loop_process(&io_socket);

        // Example condition to break the loop after some time or condition
        // You can adjust this as per your system's logic
        if (some_condition_to_exit()) {
            break;
        }
    }

    // Clean up and close the socket
    io_socket_cleanup(&io_socket);

    return 0;
}

// Placeholder function for the exit condition of the loop
int some_condition_to_exit() {
    static int count = 0;
    count++;
    return count > 5;  // For example, stop after 5 iterations
}
