#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate a random 3-SAT instance
void generate_3sat_instance(int num_variables, int num_clauses, const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    // Write the header for the 3-SAT CNF instance
    fprintf(file, "p cnf %d %d\n", num_variables, num_clauses);

    // Randomly generate clauses
    srand(time(0));  // Seed the random number generator

    for (int i = 0; i < num_clauses; ++i) {
        // Each clause consists of 3 literals, choose them randomly
        for (int j = 0; j < 3; ++j) {
            int var = rand() % num_variables + 1;  // Variable between 1 and num_variables
            if (rand() % 2 == 0) {
                var = -var;  // Randomly negate the variable
            }
            fprintf(file, "%d ", var);
        }
        fprintf(file, "0\n");  // End of clause
    }

    fclose(file);
    printf("Generated random 3-SAT instance: %s\n", filename);
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to log results to a file
void log_results(const char *solver_name, const char *instance_name, bool result, double time_taken, int iterations) {
    FILE *log_file = fopen("sat_comparison_log.txt", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    fprintf(log_file, "Solver: %s, Instance: %s, Result: %s, Time: %f seconds, Iterations: %d\n",
            solver_name, instance_name, result ? "SATISFIABLE" : "UNSATISFIABLE", time_taken, iterations);

    fclose(log_file);
}

// Function to run tests for both PMLL and MiniSat solvers
void run_comparison_tests(int num_variables, int num_clauses) {
    char filename[256];
    sprintf(filename, "3sat_instance_%d_%d.cnf", num_variables, num_clauses);

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_CLAUSES 1000
#define MAX_VARIABLES 100

// Persistent Memory Logic Loop (PMLL) to store known unsatisfiable patterns
bool pmll_unsat_patterns[MAX_CLAUSES][MAX_VARIABLES];
int pmll_unsat_count = 0;

// Function to store unsatisfiable patterns
void store_unsat_pattern(int **formula, int num_clauses, int num_variables) {
    for (int i = 0; i < num_clauses; i++) {
        for (int j = 0; j < 3; j++) {
            pmll_unsat_patterns[pmll_unsat_count][i * 3 + j] = formula[i][j];
        }
    }
    pmll_unsat_count++;
}

// Function to check if a pattern is already flagged as unsatisfiable
bool is_unsat_pattern(int **formula, int num_clauses, int num_variables) {
    for (int p = 0; p < pmll_unsat_count; p++) {
        bool match = true;
        for (int i = 0; i < num_clauses; i++) {
            for (int j = 0; j < 3; j++) {
                if (pmll_unsat_patterns[p][i * 3 + j] != formula[i][j]) {
                    match = false;
                    break;
                }
            }
            if (!match) break;
        }
        if (match) return true;
    }
    return false;
}

// Evaluate a CNF formula with a given variable assignment
bool evaluate_formula(int **formula, int *assignment, int num_clauses, int num_variables) {
    for (int i = 0; i < num_clauses; i++) {
        bool clause_satisfied = false;
        for (int j = 0; j < 3; j++) {
            int var = formula[i][j];
            if (var > 0 && assignment[var - 1] == 1) {
                clause_satisfied = true;
                break;
            }
            if (var < 0 && assignment[-var - 1] == 0) {
                clause_satisfied = true;
                break;
            }
        }
        if (!clause_satisfied) {
            return false;  // If one clause is not satisfied, formula is unsatisfied
        }
    }
    return true;  // If all clauses are satisfied
}

// Enhanced SAT Solver with PMLL, ARLL, and EFLL
bool Enhanced_SAT_Solver(int **formula, int num_clauses, int num_variables, int *assignment, int depth) {
    if (is_unsat_pattern(formula, num_clauses, num_variables)) {
        return false;  // Skip known unsatisfiable patterns
    }

    if (depth == num_variables) {
        if (evaluate_formula(formula, assignment, num_clauses, num_variables)) {
            return true;
        } else {
            store_unsat_pattern(formula, num_clauses, num_variables);
            return false;
        }
    }

    // Try assigning the current variable to 0
    assignment[depth] = 0;
    if (Enhanced_SAT_Solver(formula, num_clauses, num_variables, assignment, depth + 1)) {
        return true;
    }

    // Try assigning the current variable to 1
    assignment[depth] = 1;
    if (Enhanced_SAT_Solver(formula, num_clauses, num_variables, assignment, depth + 1)) {
        return true;
    }

    return false;  // No satisfying assignment found
}

// Helper function to generate random CNF formulas
void generate_random_cnf(int **formula, int num_clauses, int num_variables) {
    for (int i = 0; i < num_clauses; i++) {
        for (int j = 0; j < 3; j++) {
            int var = (rand() % num_variables) + 1;  // Random variable index
            if (rand() % 2 == 0) var = -var;        // Random negation
            formula[i][j] = var;
        }
    }
}

int main() {
    int num_variables;
    int num_clauses;

    // Outer loop for repeated validity checks
    for (int JKE_counter = 0; JKE_counter < 3; JKE_counter++) {
        printf("\nIteration %d:\n", JKE_counter + 1);

        // Test Case 1: Large satisfiable formula
        num_variables = 6;
        num_clauses = 4;
        int **formula1 = (int **)malloc(num_clauses * sizeof(int *));
        formula1[0] = (int[]){1, 2, 3};
        formula1[1] = (int[]){-1, 4, 5};
        formula1[2] = (int[]){-2, -3, 6};
        formula1[3] = (int[]){1, -4, 6};

        int *assignment1 = (int *)malloc(num_variables * sizeof(int));
        printf("\nTest Case 1:\n");
        if (Enhanced_SAT_Solver(formula1, num_clauses, num_variables, assignment1, 0)) {
            printf("Satisfying assignment found:\n");
            for (int i = 0; i < num_variables; i++) {
                printf("x%d = %d\n", i + 1, assignment1[i]);
            }
        } else {
            printf("No satisfying assignment exists.\n");
        }

        // Test Case 2: Contradictory formula
        num_variables = 4;
        num_clauses = 3;
        int **formula2 = (int **)malloc(num_clauses * sizeof(int *));
        formula2[0] = (int[]){1, 2};
        formula2[1] = (int[]){-1, -2};
        formula2[2] = (int[]){1, -1};

        int *assignment2 = (int *)malloc(num_variables * sizeof(int));
        printf("\nTest Case 2:\n");
        if (Enhanced_SAT_Solver(formula2, num_clauses, num_variables, assignment2, 0)) {
            printf("Satisfying assignment found:\n");
            for (int i = 0; i < num_variables; i++) {
                printf("x%d = %d\n", i + 1, assignment2[i]);
            }
        } else {
            printf("No satisfying assignment exists.\n");
        }

        // Test Case 3: Randomly generated formula
        num_variables = 8;
        num_clauses = 6;
        int **formula3 = (int **)malloc(num_clauses * sizeof(int *));
        for (int i = 0; i < num_clauses; i++) {
            formula3[i] = (int *)malloc(3 * sizeof(int));
        }
        generate_random_cnf(formula3, num_clauses, num_variables);

        int *assignment3 = (int *)malloc(num_variables * sizeof(int));
        printf("\nTest Case 3:\n");
        if (Enhanced_SAT_Solver(formula3, num_clauses, num_variables, assignment3, 0)) {
            printf("Satisfying assignment found:\n");
            for (int i = 0; i < num_variables; i++) {
                printf("x%d = %d\n", i + 1, assignment3[i]);
            }
        } else {
            printf("No satisfying assignment exists.\n");
        }

        // Free memory
        free(formula1);
        free(assignment1);
        free(formula2);
        free(assignment2);
        for (int i = 0; i < num_clauses; i++) {
            free(formula3[i]);
        }
        free(formula3);
        free(assignment3);
    }

    return 0;
}
