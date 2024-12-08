#include "api_key_signature.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Global variable to store the API key
static char apiKey[API_KEY_MAX_LENGTH];

// Initialize the API key system
void initializeApiKeySignature(const char* key) {
    if (strlen(key) >= API_KEY_MAX_LENGTH) {
        fprintf(stderr, "Error: API key exceeds maximum length.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(apiKey, key);
    printf("API key initialized.\n");
}

// Validate the API key
bool isApiKeyValid() {
    return strlen(apiKey) > 0;
}

// Broadcast transaction using the API key
void broadcastWithApiKey(const char* rawTxHex) {
    if (!isApiKeyValid()) {
        fprintf(stderr, "Error: API key is not initialized or invalid.\n");
        return;
    }

    CURL *curl = curl_easy_init();
    if (curl) {
        char postData[1024];
        snprintf(postData, sizeof(postData), "{\"tx\": \"%s\"}", rawTxHex);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char authHeader[256];
        snprintf(authHeader, sizeof(authHeader), "X-API-Token: %s", apiKey);
        headers = curl_slist_append(headers, authHeader);

        curl_easy_setopt(curl, CURLOPT_URL, API_URL);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);

        printf("Broadcasting transaction: %s\n", rawTxHex);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Curl failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Transaction Broadcast Successfully!\n");
        }

        curl_easy_cleanup(curl);
    }
}

// Cleanup API key system
void cleanupApiKeySignature() {
    memset(apiKey, 0, sizeof(apiKey)); // Clear the API key
    printf("API key system cleaned up.\n");
}
