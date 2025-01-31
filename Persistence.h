#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stdio.h>
#include <stdlib.h>
#include "memory_silo.h"
#include "PMLL_ARLL_EFLL.h"

// Assuming these types are defined in other headers:
typedef struct io_socket_t io_socket_t;
typedef struct PMLL_ARLL_EFLL_State PMLL_ARLL_EFLL_State;
typedef struct Graph Graph;

// Structure to encapsulate session context for persistence
typedef struct {
    io_socket_t io_socket;
    PMLL_ARLL_EFLL_State state;
    Graph *knowledge_graph;
} SessionContext;

// Function to save the state of the memory silo
void save_memory_silo(const char *filename, memory_silo_t *memory_silo);

// Function to load the state of the memory silo
memory_silo_t *load_memory_silo(const char *filename);

// Function to save session state
void save_session_state(const char *filename, SessionContext *ctx);

// Function to load session state
void load_session_state(const char *filename, SessionContext *ctx);

#endif // PERSISTENCE_H
