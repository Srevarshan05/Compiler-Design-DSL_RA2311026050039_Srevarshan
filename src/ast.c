/* ============================================================
 * ast.c — Abstract Syntax Tree implementation
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* ----------------------------------------------------------
 * Helper: allocate and zero a new node
 * ---------------------------------------------------------- */
static ASTNode *new_node(NodeType type) {
    ASTNode *n = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (!n) {
        fprintf(stderr, "[AST] Out of memory\n");
        exit(1);
    }
    n->type = type;
    return n;
}

/* ----------------------------------------------------------
 * create_num_node — integer literal node
 * ---------------------------------------------------------- */
ASTNode *create_num_node(int value) {
    ASTNode *n  = new_node(NODE_NUM);
    n->num_val  = value;
    return n;
}

/* ----------------------------------------------------------
 * create_var_node — variable reference node
 * ---------------------------------------------------------- */
ASTNode *create_var_node(const char *name) {
    ASTNode *n = new_node(NODE_VAR);
    strncpy(n->name, name, sizeof(n->name) - 1);
    return n;
}

/* ----------------------------------------------------------
 * create_binop_node — binary operation node
 * ---------------------------------------------------------- */
ASTNode *create_binop_node(char op, ASTNode *left, ASTNode *right) {
    ASTNode *n = new_node(NODE_BINOP);
    n->op      = op;
    n->left    = left;
    n->right   = right;
    return n;
}

/* ----------------------------------------------------------
 * create_decl_node — declaration node:  int name = expr
 * ---------------------------------------------------------- */
ASTNode *create_decl_node(const char *name, ASTNode *expr) {
    ASTNode *n = new_node(NODE_DECL);
    strncpy(n->name, name, sizeof(n->name) - 1);
    n->left    = expr;   /* expression on RHS */
    return n;
}

/* ----------------------------------------------------------
 * create_print_node — print statement node
 * ---------------------------------------------------------- */
ASTNode *create_print_node(ASTNode *expr) {
    ASTNode *n = new_node(NODE_PRINT);
    n->left    = expr;
    return n;
}

/* ----------------------------------------------------------
 * print_ast — pretty-print the tree with indentation
 * ---------------------------------------------------------- */
void print_ast(ASTNode *node, int indent) {
    if (!node) return;

    /* Indent proportional to depth */
    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case NODE_PROGRAM:
            printf("[PROGRAM]\n");
            print_ast(node->left, indent + 1);
            break;

        case NODE_DECL:
            printf("[DECL] int %s =\n", node->name);
            print_ast(node->left, indent + 1);
            break;

        case NODE_ASSIGN:
            printf("[ASSIGN] %s =\n", node->name);
            print_ast(node->left, indent + 1);
            break;

        case NODE_PRINT:
            printf("[PRINT]\n");
            print_ast(node->left, indent + 1);
            break;

        case NODE_BINOP:
            printf("[BINOP] '%c'\n", node->op);
            print_ast(node->left,  indent + 1);
            print_ast(node->right, indent + 1);
            break;

        case NODE_NUM:
            printf("[NUM] %d\n", node->num_val);
            break;

        case NODE_VAR:
            printf("[VAR] %s\n", node->name);
            break;

        default:
            printf("[UNKNOWN NODE]\n");
    }

    /* Traverse the sibling chain (statement list) */
    if (node->next) {
        print_ast(node->next, indent);
    }
}

/* ----------------------------------------------------------
 * free_ast — recursively free every node
 * ---------------------------------------------------------- */
void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);
    free(node);
}
