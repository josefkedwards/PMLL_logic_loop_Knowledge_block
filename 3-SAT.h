#ifndef THREE_SAT_H
#define THREE_SAT_H

#include <stdbool.h>

#define MAX_CLAUSES 1000
#define MAX_VARIABLES 100

// Persistent Memory Logic Loop (PMLL) to store known unsatisfiable patterns
extern bool pmll_unsat_patterns[MAX_CLAUSES][MAX_VARIABLES];
extern int pmll_unsat_count;

// Function to store unsatisfiable patterns
void store_unsat_pattern(int **formula, int num_clauses, int num_variables);

// Function to check if a pattern is already flagged as unsatisfiable
bool is_unsat_pattern(int **formula, int num_clauses, int num_variables);

// Evaluate a CNF formula with a given variable assignment
bool evaluate_formula(int **formula, int *assignment, int num_clauses, int num_variables);

// Enhanced SAT Solver with PMLL, ARLL, and EFLL
bool Enhanced_SAT_Solver(int **formula, int num_clauses, int num_variables, int *assignment, int depth);

// Helper function to generate random CNF formulas
void generate_random_cnf(int **formula, int num_clauses, int num_variables);

// Function to generate a random 3-SAT instance and save to a file
void generate_3sat_instance(int num_variables, int num_clauses, const char* filename);

// Function to log results to a file
void log_results(const char *solver_name, const char *instance_name, bool result, double time_taken, int iterations);

#endif // THREE_SAT_H
