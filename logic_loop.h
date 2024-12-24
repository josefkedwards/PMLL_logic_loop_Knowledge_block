#ifndef LOGIC_LOOPS_H
#define LOGIC_LOOPS_H

#include <stdbool.h>

// Maximum retry attempts for ARLL
#define RETRY_LIMIT 3

// Error flags for EFLL
#define EFLL_ERROR_NONE 0
#define EFLL_ERROR_MEMORY 1
#define EFLL_ERROR_IO 2
#define EFLL_ERROR_UNKNOWN 3

// Structure for logic loops
typedef struct {
    int id;
    int memory_silo_id;
    int io_socket_id;
    int free_c_present;  // Flag indicating presence of free.c
    int retries;         // Retry count for ARLL
    int error_flag;      // Error flag for EFLL
} logic_loop_t;

/**
 * Initialize the PML logic loop.
 * @param memory_silo_id ID of the memory silo.
 * @param io_socket_id ID of the IO socket.
 */
void pml_logic_loop_init(int memory_silo_id, int io_socket_id);

/**
 * Process logic within the PML logic loop.
 * @param io_socket_id The socket ID.
 * @param buffer Pointer to the buffer.
 * @param length Length of the buffer.
 */
void pml_logic_loop_process(int io_socket_id, void* buffer, int length);

/**
 * Apply ARLL to retry failed operations.
 * @param operation A function pointer to the operation to retry.
 * @param args Pointer to arguments for the operation.
 * @return True if the operation succeeds, false otherwise.
 */
bool arll_retry(bool (*operation)(void*), void* args);

/**
 * Handle errors using EFLL.
 * @param error_flag The error flag.
 */
void efll_handle_error(int error_flag);

/**
 * Get the current state of the PML logic loop.
 * @param io_socket_id The socket ID.
 * @return Pointer to the current logic loop state.
 */
logic_loop_t* get_pml_logic_loop(int io_socket_id);

/**
 * Cleanup memory and socket resources for the logic loop.
 */
void logic_loop_cleanup();

#endif // LOGIC_LOOPS_H
