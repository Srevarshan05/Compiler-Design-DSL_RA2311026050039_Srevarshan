/* ============================================================
 * main.c — Compiler Driver
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 *
 * Pipeline:
 *   stdin → Lex/Parse → AST → Semantic Analysis → ICG → stdout
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"
#include "icg.h"

/* Declared in parser.y */
extern int      yyparse(void);
extern ASTNode *ast_root;

/* ============================================================
 * main
 * ============================================================ */
int main(void) {

    printf("============================================================\n");
    printf("  Mini DSL Compiler                                        \n");
    printf("  Author: Srevarshan (RA2311026050039)                     \n");
    printf("============================================================\n\n");

    /* --------------------------------------------------------
     * Phase 1: Lexical Analysis + Parsing
     * lex/bison handle this jointly through yyparse()
     * -------------------------------------------------------- */
    printf("----- Phase 1: Lexical Analysis & Parsing -----\n");
    int parse_result = yyparse();

    if (parse_result != 0 || ast_root == NULL) {
        fprintf(stderr, "[COMPILER] Parsing failed. Aborting.\n");
        return 1;
    }
    printf("\n[PARSER] Parsing completed successfully.\n\n");

    /* --------------------------------------------------------
     * Phase 2: AST Print
     * -------------------------------------------------------- */
    printf("----- Phase 2: Abstract Syntax Tree -----\n");
    print_ast(ast_root, 0);
    printf("\n");

    /* --------------------------------------------------------
     * Phase 3: Semantic Analysis
     * -------------------------------------------------------- */
    printf("----- Phase 3: Semantic Analysis -----\n");
    init_symbol_table();
    int errors = run_semantic_analysis(ast_root);
    print_symbol_table();

    if (errors > 0) {
        fprintf(stderr,
            "[COMPILER] Semantic errors found. Aborting code generation.\n");
        free_ast(ast_root);
        return 1;
    }

    /* --------------------------------------------------------
     * Phase 4: Intermediate Code Generation (TAC)
     * -------------------------------------------------------- */
    printf("----- Phase 4: Intermediate Code Generation -----\n");
    init_icg();
    generate_tac(ast_root);
    print_tac();

    /* --------------------------------------------------------
     * Clean up and exit
     * -------------------------------------------------------- */
    free_ast(ast_root);

    printf("============================================================\n");
    printf("  Compilation finished successfully.\n");
    printf("============================================================\n");

    return 0;
}
