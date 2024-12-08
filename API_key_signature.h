#ifndef API_KEY_SIGNATURE_H
#define API_KEY_SIGNATURE_H

#include <stdbool.h>

// Define constants
#define API_KEY_MAX_LENGTH 128
#define API_URL "https://api.blockchain.com/v3/exchange/transactions"

// Function prototypes
void initializeApiKeySignature(const char* apiKey);       // Initialize the API key system
bool isApiKeyValid();                                     // Validate the API key
void broadcastWithApiKey(const char* rawTxHex);           // Broadcast transaction using API key
void cleanupApiKeySignature();                            // Cleanup API key system

#endif // API_KEY_SIGNATURE_H
