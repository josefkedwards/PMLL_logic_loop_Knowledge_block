#ifndef CUSTODIAN_H
#define CUSTODIAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io_socket.h"

// Define any constants or macros
#define MAX_CUSTODIAN_NAME_LENGTH 100
#define CUSTODIAN_STATUS_ACTIVE 1
#define CUSTODIAN_STATUS_INACTIVE 0

// Custodian structure
typedef struct {
    int id;                                 // Unique identifier for the custodian
    char name[MAX_CUSTODIAN_NAME_LENGTH];  // Custodian's name
    int status;                            // Active or inactive status
    int io_socket_id;                      // Associated IO socket ID for communication
} custodian_t;

// Function prototypes
void custodian_init(custodian_t* custodian, int id, const char* name, int io_socket_id);
void custodian_set_status(custodian_t* custodian, int status);
int custodian_get_status(const custodian_t* custodian);
void custodian_send_message(const custodian_t* custodian, const char* message);
void custodian_receive_message(const custodian_t* custodian, char* buffer, size_t buffer_size);

#endif // CUSTODIAN_H