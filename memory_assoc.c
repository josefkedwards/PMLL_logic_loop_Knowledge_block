#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 50
#define MAX_MEMORY_NODES 100

typedef struct MemoryNode {
    char token[MAX_TOKEN_LENGTH];
    struct MemoryNode* next;
} MemoryNode;

typedef struct MemoryGraph {
    MemoryNode* nodes[MAX_MEMORY_NODES];
    int size;
} MemoryGraph;

// Function to create a new memory node
MemoryNode* createNode(const char* token) {
    MemoryNode* node = (MemoryNode*)malloc(sizeof(MemoryNode));
    strcpy(node->token, token);
    node->next = NULL;
    return node;
}

// Initialize the memory graph
void initializeGraph(MemoryGraph* graph) {
    for (int i = 0; i < MAX_MEMORY_NODES; i++) {
        graph->nodes[i] = NULL;
    }
    graph->size = 0;
}

// Hash function for storing tokens
int hash(const char* token) {
    int hashValue = 0;
    for (int i = 0; token[i] != '\0'; i++) {
        hashValue = (hashValue + token[i]) % MAX_MEMORY_NODES;
    }
    return hashValue;
}

// Add a token to the memory graph
void addToken(MemoryGraph* graph, const char* token) {
    int index = hash(token);
    MemoryNode* current = graph->nodes[index];
    while (current != NULL) {
        if (strcmp(current->token, token) == 0) {
            return; // Token already exists
        }
        current = current->next;
    }
    MemoryNode* newNode = createNode(token);
    newNode->next = graph->nodes[index];
    graph->nodes[index] = newNode;
    graph->size++;
}

// Tokenizer function
void tokenizeAndAdd(MemoryGraph* graph, const char* input) {
    char buffer[MAX_TOKEN_LENGTH];
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == ' ' || input[i] == ',' || input[i] == '.') {
            if (j > 0) {
                buffer[j] = '\0';
                addToken(graph, buffer);
                j = 0;
            }
        } else {
            buffer[j++] = input[i];
        }
    }
    if (j > 0) {
        buffer[j] = '\0';
        addToken(graph, buffer);
    }
}

// Display memory nodes and their associations
void displayGraph(const MemoryGraph* graph) {
    printf("Memory Graph:\n");
    for (int i = 0; i < MAX_MEMORY_NODES; i++) {
        if (graph->nodes[i] != NULL) {
            MemoryNode* current = graph->nodes[i];
            printf("[%d] -> ", i);
            while (current != NULL) {
                printf("%s -> ", current->token);
                current = current->next;
            }
            printf("NULL\n");
        }
    }
}

int main() {
    MemoryGraph graph;
    initializeGraph(&graph);
    
    // Sample input text
    const char* input = "Josef is the creator of the PMLL and PMRLL logic loops.";

    // Tokenize and add to memory
    tokenizeAndAdd(&graph, input);

    // Display memory associations
    displayGraph(&graph);

    return 0;
}
