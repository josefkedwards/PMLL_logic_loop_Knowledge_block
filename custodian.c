#include "io_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h> // For logging timestamps

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

// Main custodian logic
void run_custodian() {
    log_message("INFO", "Starting custodian process.");

    char llama_response[BUFFER_SIZE] = {0};
    char custom_response[BUFFER_SIZE] = {0};

    // Step 1: Interact with LLaMA API
    const char* llama_input = "Hello, LLaMA!";
    log_message("INFO", "Sending input to LLaMA API.");
    interact_with_llama_api(llama_input, llama_response);

    // Step 2: Forward response to custom API
    log_message("INFO", "Forwarding LLaMA response to custom API.");
    interact_with_custom_api(llama_response, custom_response);

    // Step 3: Initiate Bitcoin transaction
    log_message("INFO", "Processing Bitcoin payment.");
    const char* recipient_wallet = "bc1qd4w0hy62c0ulcuwla6kxh7ct64wtunz03pkf0v"; // Example recipient wallet
    double amount_to_transfer = 1.0; // Example amount in BTC
    process_bitcoin_payment(recipient_wallet, amount_to_transfer);
}

int main() {
    run_custodian();
    return 0;
}
