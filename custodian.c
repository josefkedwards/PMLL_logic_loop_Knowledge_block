#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <time.h>
#include "io_socket.h"
#include "memory_silo.h"
#include "pml_logic_loop.h"
#include "knowledge.h"

#define LLAMA_API_URL "https://api.llama.ai/v1/chat" // LLaMA API endpoint
#define LLAMA_API_KEY "LA-8c4003a74c5040b2b735866f22e754ed55c2ab712b0346b3bca0f1993362704a"
#define CUSTOM_API_URL "http://127.0.0.1:8080/api/v1" // Custom API endpoint
#define BITCOIN_WALLET "bc1qetkudft7hlsl3k7nhrg6zrkufpu6q3rdnx5ag5" // Your Bitcoin wallet address
#define BUFFER_SIZE 1024
#define LOG_FILE "custodian.log"

// Logging utility
void log_message(const char* level, const char* message) {
    FILE* log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] [%s] %s\n", level, ctime(&now), message);
        fclose(log_file);
    }
}

// CURL Write callback for capturing API responses
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    strncat((char*)userp, (char*)contents, realsize);
    return realsize;
}

// Function to interact with the LLaMA API
void interact_with_llama_api(const char* input_prompt, char* llama_response) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        log_message("ERROR", "Failed to initialize cURL for LLaMA API.");
        return;
    }

    char post_fields[BUFFER_SIZE];
    snprintf(post_fields, sizeof(post_fields), "{\"prompt\": \"%s\", \"max_tokens\": 50}", input_prompt);

    char response_buffer[BUFFER_SIZE] = {0};

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Authorization: Bearer " LLAMA_API_KEY);

    curl_easy_setopt(curl, CURLOPT_URL, LLAMA_API_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        log_message("ERROR", curl_easy_strerror(res));
    } else {
        snprintf(llama_response, BUFFER_SIZE, "%s", response_buffer);
        log_message("INFO", "Successfully interacted with LLaMA API.");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

// Function to interact with the custom API
void interact_with_custom_api(const char* input_data, char* custom_response) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        log_message("ERROR", "Failed to initialize cURL for custom API.");
        return;
    }

    char post_fields[BUFFER_SIZE];
    snprintf(post_fields, sizeof(post_fields), "{\"data\": \"%s\"}", input_data);

    char response_buffer[BUFFER_SIZE] = {0};

    curl_easy_setopt(curl, CURLOPT_URL, CUSTOM_API_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        log_message("ERROR", curl_easy_strerror(res));
    } else {
        snprintf(custom_response, BUFFER_SIZE, "%s", response_buffer);
        log_message("INFO", "Successfully interacted with custom API.");
    }

    curl_easy_cleanup(curl);
}

// Function to process Bitcoin payments
void process_bitcoin_payment(const char* recipient, double amount) {
    log_message("INFO", "Initiating Bitcoin transaction.");

    char transaction_command[BUFFER_SIZE];
    snprintf(transaction_command, sizeof(transaction_command),
             "bitcoin-cli sendtoaddress \"%s\" %.8f", recipient, amount);

    int result = system(transaction_command);
    if (result == 0) {
        log_message("INFO", "Bitcoin transaction successful.");
    } else {
        log_message("ERROR", "Bitcoin transaction failed.");
    }
}

// Function to sync with memory silo
void sync_with_memory_silo(io_socket_t* io_socket, memory_silo_t* silo) {
    if (!io_socket || !silo) {
        log_message("ERROR", "Invalid parameters for memory silo synchronization.");
        return;
    }

    if (io_socket_send(io_socket->socket, silo->memory, silo->size) < 0) {
        log_message("ERROR", "Failed to sync memory silo data.");
    } else {
        log_message("INFO", "Memory silo data synced successfully.");
    }

    char response[BUFFER_SIZE];
    if (io_socket_receive(io_socket->socket, response, sizeof(response)) > 0) {
        log_message("INFO", "Response received from memory silo synchronization.");
    }
}

// Main custodian logic
void run_custodian() {
    log_message("INFO", "Starting custodian process.");

    // Step 1: Initialize IO socket
    io_socket_t io_socket;
    if (io_socket_init(&io_socket, "127.0.0.1", 8080) < 0) {
        log_message("ERROR", "Failed to initialize IO socket.");
        return;
    }

    // Step 2: Initialize memory silo
    memory_silo_t* silo = malloc(sizeof(memory_silo_t));
    if (!silo) {
        log_message("ERROR", "Failed to allocate memory for silo.");
        io_socket_cleanup(&io_socket);
        return;
    }
    memory_silo_init(silo->io_socket);

    // Step 3: Interact with APIs
    char llama_response[BUFFER_SIZE] = {0};
    char custom_response[BUFFER_SIZE] = {0};
    const char* llama_input = "Hello, LLaMA!";
    interact_with_llama_api(llama_input, llama_response);
    interact_with_custom_api(llama_response, custom_response);

    // Step 4: Execute PML Logic Loop
    log_message("INFO", "Executing PML Logic Loop.");
    pml_logic_loop_process(io_socket.socket, silo->memory, silo->size);

    // Step 5: Process Bitcoin transaction
    log_message("INFO", "Processing Bitcoin payment.");
    process_bitcoin_payment(BITCOIN_WALLET, 0.1);

    // Step 6: Sync with memory silo
    sync_with_memory_silo(&io_socket, silo);

    // Cleanup resources
    free(silo);
    io_socket_cleanup(&io_socket);
    log_message("INFO", "Custodian process completed.");
}

// Main function
int main() {
    run_custodian();
    return 0;
}
