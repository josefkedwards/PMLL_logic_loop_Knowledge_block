#ifndef MEMORY_SILO_H
#define MEMORY_SILO_H

#include <stdlib.h>
#include <stddef.h>

// Memory Silo Structure
typedef struct {
    int id;
    int io_socket;
    void* memory;
    size_t size;
} memory_silo_t;

// Function Declarations
void memory_silo_init(int io_socket);
memory_silo_t* get_memory_silo(int io_socket);
void memory_silo_write(int io_socket, void* buffer, size_t length);
void memory_silo_read(int io_socket, void* buffer, size_t length);
void cleanup_memory_silos();

#endif // MEMORY_SILO_H