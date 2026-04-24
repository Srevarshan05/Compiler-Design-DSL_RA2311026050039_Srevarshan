/* ============================================================
 * icg.h — Intermediate Code Generation (Three-Address Code)
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 * ============================================================ */

#ifndef ICG_H
#define ICG_H

#include "ast.h"

/* ----------------------------------------------------------
 * TAC instruction types
 * ---------------------------------------------------------- */
typedef enum {
    TAC_ASSIGN,   /* result = arg1                         */
    TAC_BINOP,    /* result = arg1 OP arg2                 */
    TAC_PRINT     /* print result                          */
} TACType;

/* ----------------------------------------------------------
 * A single Three-Address Code instruction
 * ---------------------------------------------------------- */
typedef struct {
    TACType type;
    char    result[64];
    char    arg1[64];
    char    arg2[64];
    char    op;          /* +, -, *, /  (for TAC_BINOP)   */
} TACInstr;

/* ----------------------------------------------------------
 * TAC program (simple array)
 * ---------------------------------------------------------- */
#define MAX_TAC 512

/* ----------------------------------------------------------
 * Public API
 * ---------------------------------------------------------- */
void  init_icg(void);
char *gen_code(ASTNode *node);   /* returns result operand  */
void  gen_statement(ASTNode *node);
void  generate_tac(ASTNode *root);
void  print_tac(void);

#endif /* ICG_H */
