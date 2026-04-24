/* ============================================================
 * semantic.h — Symbol Table and Semantic Checker declarations
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 * ============================================================ */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

/* ----------------------------------------------------------
 * Symbol table entry
 * ---------------------------------------------------------- */
#define MAX_SYMBOLS 256

typedef struct {
    char name[64];   /* variable name    */
    int  value;      /* stored value     */
    int  initialized;/* 1 if assigned    */
} Symbol;

/* ----------------------------------------------------------
 * Public API
 * ---------------------------------------------------------- */
void init_symbol_table(void);
int  declare_symbol(const char *name);   /* returns 0=ok, -1=duplicate */
int  lookup_symbol(const char *name);    /* returns index or -1        */
void set_symbol_value(const char *name, int val);
int  get_symbol_value(const char *name);

/* Runs semantic analysis on the whole AST.
   Returns number of errors found (0 = clean). */
int  run_semantic_analysis(ASTNode *root);

/* Pretty-print the symbol table */
void print_symbol_table(void);

#endif /* SEMANTIC_H */
