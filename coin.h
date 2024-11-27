#ifndef COIN_H
#define COIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Bitcoin transaction structure
typedef struct {
    const char *wallet_address;
    unsigned char *bitcoin;
} bitcoin_transaction_t;

// Function declarations for Bitcoin transaction handling
void install_dependencies();  // Installs required dependencies for Bitcoin operations
void execute_secure_command(const char *command);  // Executes system commands with sudo
void send_bitcoin_to_wallet(const char *wallet_address, unsigned char *bitcoin);  // Sends Bitcoin to the specified wallet
void PMLL_Hash_Function(const char *input, unsigned char *output);  // Hashes input data
void PMLL_Compress_Data(const char *input, unsigned char *output);  // Compresses data with AES and SHA-256
void PMLL_Cache_Data(const char *input, unsigned char *output);  // Caches data for later use

// Function for Bitcoin transaction signing
void sign_bitcoin_transaction(char* tx_hex, const char* private_key);

// Function to send signed Bitcoin transaction to the Bitcoin network
int send_bitcoin_transaction(const char* raw_tx);

// Function to process Bitcoin payments
void process_bitcoin_payment(const char* recipient, double amount);

// Function to create and sign a Bitcoin transaction
void create_and_sign_transaction(const char* recipient_address, double amount);

#endif // COIN_H
