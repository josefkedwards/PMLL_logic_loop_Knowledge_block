#ifndef IBC_H
#define IBC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRANSACTIONS 100
#define MAX_CHAIN_NAME 50

// Blockchain Transaction Structure
typedef struct {
    char tx_id[64];
    char source_chain[MAX_CHAIN_NAME];
    char destination_chain[MAX_CHAIN_NAME];
    char asset[10];
    double amount;
    char status[15];
    int suspicious;
} Transaction;

// IBC Ledger Structure
typedef struct {
    Transaction transactions[MAX_TRANSACTIONS];
    int transaction_count;
} IBCLedger;

// Function Prototypes
void init_ibc_ledger(IBCLedger *ledger);
void add_transaction(IBCLedger *ledger, const char *tx_id, const char *source, const char *destination, 
                     const char *asset, double amount, const char *status, int suspicious);
void print_ledger(const IBCLedger *ledger);
void detect_suspicious_transactions(const IBCLedger *ledger);

#endif
