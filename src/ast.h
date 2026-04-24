/* ============================================================
 * ast.h — Abstract Syntax Tree: types and declarations
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 * ============================================================ */

#ifndef AST_H
#define AST_H

/* ----------------------------------------------------------
 * Node type enumeration
 * ---------------------------------------------------------- */
typedef enum {
    NODE_PROGRAM,       /* root: list of statements          */
    NODE_DECL,          /* int x = expr;                     */
    NODE_ASSIGN,        /* x = expr;   (future extension)    */
    NODE_PRINT,         /* print(expr);                      */
    NODE_BINOP,         /* left OP right                     */
    NODE_NUM,           /* integer literal                   */
    NODE_VAR            /* variable reference                */
} NodeType;

/* ----------------------------------------------------------
 * AST node structure
 * ---------------------------------------------------------- */
typedef struct ASTNode {
    NodeType type;

    /* For NODE_BINOP: operator character (+, -, *, /)        */
    char op;

    /* For NODE_NUM: integer value                            */
    int  num_val;

    /* For NODE_VAR / NODE_DECL: variable name               */
    char name[64];

    /* Children (up to 3 slots cover all grammar rules)      */
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next;   /* linked-list of statements     */
} ASTNode;

/* ----------------------------------------------------------
 * Public API
 * ---------------------------------------------------------- */
ASTNode *create_num_node(int value);
ASTNode *create_var_node(const char *name);
ASTNode *create_binop_node(char op, ASTNode *left, ASTNode *right);
ASTNode *create_decl_node(const char *name, ASTNode *expr);
ASTNode *create_print_node(ASTNode *expr);
void     print_ast(ASTNode *node, int indent);
void     free_ast(ASTNode *node);

#endif /* AST_H */
