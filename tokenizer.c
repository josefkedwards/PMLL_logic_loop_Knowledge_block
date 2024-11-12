#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"

#define MAX_TOKEN_SIZE 100

void tokenize(char *input) {
    char *token = strtok(input, " \t\n");
    while (token != NULL) {
        printf("Token: %s\n", token);
        token = strtok(NULL, " \t\n");
    }
}

#ifndef TOKENIZER_H
#define TOKENIZER_H

void tokenize(char *input);

#endif // TOKENIZER_H
```

#### main.c
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"

#define MAX_TOKEN_SIZE 100

int main() {
    char input[MAX_TOKEN_SIZE];

    printf("Enter a string to tokenize: ");
    fgets(input, MAX_TOKEN_SIZE, stdin);
    input[strcspn(input, "\n")] = '\0';

    tokenize(input);

    return 0;
}
