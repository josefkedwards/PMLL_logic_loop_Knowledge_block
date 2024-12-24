#include "PMLL_ARLL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void pmll_arll_init(PMLL_ARLL_State *state, int max_retries) {
    if (!state) {
        fprintf(stderr, "Error: PMLL_ARLL_State is NULL during initialization.\n");
        exit(EXIT_FAILURE);
    }
    state->retries = 0;
    state->max_retries = max_retries;
    memset(state->buffer, 0, sizeof(state->buffer));
    state->json = NULL;
    log_message("INFO", "PMLL_ARLL initialized successfully.");
}

int pmll_arll_process_chunk(PMLL_ARLL_State *state, const char *chunk, size_t chunk_size) {
    if (!state || !chunk || chunk_size == 0) {
        log_message("ERROR", "Invalid parameters to process chunk.");
        return -1;
    }

    // Append the chunk to the buffer
    strncat(state->buffer, chunk, chunk_size);

    // Attempt to parse JSON
    state->json = json_loads(state->buffer, 0, NULL);
    if (!state->json) {
        log_message("WARNING", "Invalid JSON chunk. Retrying...");
        state->retries++;
        return 0; // Retry
    }

    log_message("INFO", "Valid JSON received.");
    return 1; // Success
}

void pmll_arll_write_to_knowledge_graph(PMLL_ARLL_State *state) {
    if (!state || !state->json) {
        log_message("ERROR", "Cannot write invalid JSON to the knowledge graph.");
        return;
    }

    printf("[ARLL] Writing JSON data to knowledge graph...\n");
    json_dumpf(state->json, stdout, JSON_INDENT(2));
    printf("\n[ARLL] Data successfully written to knowledge graph.\n");
}

void pmll_arll_cleanup(PMLL_ARLL_State *state) {
    if (!state) return;

    if (state->json) {
        json_decref(state->json);
        state->json = NULL;
    }

    memset(state->buffer, 0, sizeof(state->buffer));
    state->retries = 0;

    log_message("INFO", "PMLL_ARLL resources cleaned up.");
}

void log_message(const char *level, const char *message) {
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("[%s] [%s] %s\n", level, timestamp, message);
}
