#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_TOKEN_SIZE 100

// Function to check if a byte is the start of a UTF-8 character
bool is_utf8_start_byte(unsigned char byte) {
    return (byte & 0xC0) != 0x80; // Start byte does not have the pattern 10xxxxxx
}

// UTF-8 Tokenizer function
void utf8_tokenize(const char *input) {
    const char *delimiters = " \t\n";
    const char *start = input;
    const char *current = input;

    printf("Tokenizing input: \"%s\"\n", input);

    while (*current) {
        // Check if the current character is a delimiter
        if (strchr(delimiters, *current) != NULL) {
            // Print the token if the length is greater than 0
            if (current > start) {
                printf("Token: ");
                fwrite(start, 1, current - start, stdout);
                printf("\n");
            }
            // Skip the delimiter and set the start of the next token
            current++;
            start = current;
        } else {
            // Move to the next byte in the UTF-8 sequence
            if (is_utf8_start_byte((unsigned char)*current)) {
                current++;
                while (*current && (*current & 0xC0) == 0x80) {
                    current++; // Continue through the multi-byte character
                }
            } else {
                current++;
            }
        }
    }

    // Print the last token if there is one
    if (current > start) {
        printf("Token: ");
        fwrite(start, 1, current - start, stdout);
        printf("\n");
    }
}

int main() {
    char input[MAX_TOKEN_SIZE];

    printf("Enter a string to tokenize (supports UTF-8): ");
    fgets(input, MAX_TOKEN_SIZE, stdin);
    input[strcspn(input, "\n")] = '\0'; // Remove the newline character

    utf8_tokenize(input);

    return 0;
}

