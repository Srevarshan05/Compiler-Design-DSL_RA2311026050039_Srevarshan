/* ============================================================
 * semantic.c — Symbol Table & Semantic Analysis
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include "semantic.h"

/* ----------------------------------------------------------
 * Module-private state
 * ---------------------------------------------------------- */
static Symbol sym_table[MAX_SYMBOLS];
static int    sym_count = 0;
static int    sem_errors = 0;

/* ----------------------------------------------------------
 * init_symbol_table — reset table before a new compilation
 * ---------------------------------------------------------- */
void init_symbol_table(void) {
    sym_count  = 0;
    sem_errors = 0;
    memset(sym_table, 0, sizeof(sym_table));
}

/* ----------------------------------------------------------
 * lookup_symbol — returns index in table, or -1 if absent
 * ---------------------------------------------------------- */
int lookup_symbol(const char *name) {
    for (int i = 0; i < sym_count; i++) {
        if (strcmp(sym_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* ----------------------------------------------------------
 * declare_symbol — adds a new symbol; error on duplicate
 * Returns 0 on success, -1 on duplicate
 * ---------------------------------------------------------- */
int declare_symbol(const char *name) {
    if (lookup_symbol(name) != -1) {
        fprintf(stderr,
            "[SEMANTIC ERROR] Variable '%s' already declared.\n", name);
        sem_errors++;
        return -1;
    }
    if (sym_count >= MAX_SYMBOLS) {
        fprintf(stderr, "[SEMANTIC ERROR] Symbol table full.\n");
        sem_errors++;
        return -1;
    }
    strncpy(sym_table[sym_count].name, name,
            sizeof(sym_table[sym_count].name) - 1);
    sym_table[sym_count].value       = 0;
    sym_table[sym_count].initialized = 0;
    sym_count++;
    return 0;
}

/* ----------------------------------------------------------
 * set_symbol_value — mark a symbol as initialised + store val
 * ---------------------------------------------------------- */
void set_symbol_value(const char *name, int val) {
    int idx = lookup_symbol(name);
    if (idx != -1) {
        sym_table[idx].value       = val;
        sym_table[idx].initialized = 1;
    }
}

/* ----------------------------------------------------------
 * get_symbol_value — retrieve stored value
 * ---------------------------------------------------------- */
int get_symbol_value(const char *name) {
    int idx = lookup_symbol(name);
    if (idx != -1) return sym_table[idx].value;
    return 0;
}

/* ----------------------------------------------------------
 * Internal: check a single expression sub-tree
 * ---------------------------------------------------------- */
static void check_expr(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_VAR:
            /* Variable must be declared before use */
            if (lookup_symbol(node->name) == -1) {
                fprintf(stderr,
                    "[SEMANTIC ERROR] Variable '%s' used before declaration.\n",
                    node->name);
                sem_errors++;
            }
            break;

        case NODE_NUM:
            /* Literals are always fine */
            break;

        case NODE_BINOP:
            check_expr(node->left);
            check_expr(node->right);
            break;

        default:
            break;
    }
}

/* ----------------------------------------------------------
 * Internal: check one statement node
 * ---------------------------------------------------------- */
static void check_statement(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_DECL:
            /* Check the initialiser expression first,
               then declare the variable (prevents  int a = a;) */
            check_expr(node->left);
            declare_symbol(node->name);
            break;

        case NODE_ASSIGN:
            /* Target must already be declared */
            if (lookup_symbol(node->name) == -1) {
                fprintf(stderr,
                    "[SEMANTIC ERROR] Assignment to undeclared variable '%s'.\n",
                    node->name);
                sem_errors++;
            }
            check_expr(node->left);
            break;

        case NODE_PRINT:
            check_expr(node->left);
            break;

        default:
            break;
    }
}

/* ----------------------------------------------------------
 * run_semantic_analysis — walk the statement list
 * ---------------------------------------------------------- */
int run_semantic_analysis(ASTNode *root) {
    sem_errors = 0;

    /* The root can be a NODE_PROGRAM wrapper or the first stmt */
    ASTNode *stmt = (root && root->type == NODE_PROGRAM)
                    ? root->left
                    : root;

    while (stmt) {
        check_statement(stmt);
        stmt = stmt->next;
    }

    if (sem_errors == 0) {
        printf("[SEMANTIC] No errors found. Program is valid.\n");
    } else {
        printf("[SEMANTIC] %d error(s) detected.\n", sem_errors);
    }
    return sem_errors;
}

/* ----------------------------------------------------------
 * print_symbol_table — display for debugging / viva
 * ---------------------------------------------------------- */
void print_symbol_table(void) {
    printf("\n===== Symbol Table =====\n");
    printf("%-20s %-10s %-12s\n", "Variable", "Value", "Initialized");
    printf("--------------------------------------------\n");
    for (int i = 0; i < sym_count; i++) {
        printf("%-20s %-10d %-12s\n",
               sym_table[i].name,
               sym_table[i].value,
               sym_table[i].initialized ? "yes" : "no");
    }
    printf("========================================\n\n");
}
