/* ============================================================
 * icg.c — Three-Address Code (TAC) Generation
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icg.h"

/* ----------------------------------------------------------
 * Module-private TAC program buffer
 * ---------------------------------------------------------- */
static TACInstr tac_program[MAX_TAC];
static int      tac_count    = 0;
static int      temp_counter = 0;   /* t0, t1, t2 … */

/* ----------------------------------------------------------
 * init_icg — reset state before code generation
 * ---------------------------------------------------------- */
void init_icg(void) {
    tac_count    = 0;
    temp_counter = 0;
    memset(tac_program, 0, sizeof(tac_program));
}

/* ----------------------------------------------------------
 * Helper: generate a fresh temporary name like "t0", "t1"
 * ---------------------------------------------------------- */
static void new_temp(char *buf) {
    sprintf(buf, "t%d", temp_counter++);
}

/* ----------------------------------------------------------
 * Helper: emit a TAC_ASSIGN instruction
 *   result = arg1
 * ---------------------------------------------------------- */
static void emit_assign(const char *result, const char *arg1) {
    TACInstr *ins = &tac_program[tac_count++];
    ins->type     = TAC_ASSIGN;
    strncpy(ins->result, result, 63);
    strncpy(ins->arg1,   arg1,   63);
}

/* ----------------------------------------------------------
 * Helper: emit a TAC_BINOP instruction
 *   result = arg1 OP arg2
 * ---------------------------------------------------------- */
static void emit_binop(const char *result, const char *arg1,
                        char op,            const char *arg2) {
    TACInstr *ins = &tac_program[tac_count++];
    ins->type     = TAC_BINOP;
    ins->op       = op;
    strncpy(ins->result, result, 63);
    strncpy(ins->arg1,   arg1,   63);
    strncpy(ins->arg2,   arg2,   63);
}

/* ----------------------------------------------------------
 * Helper: emit a TAC_PRINT instruction
 * ---------------------------------------------------------- */
static void emit_print(const char *arg) {
    TACInstr *ins = &tac_program[tac_count++];
    ins->type     = TAC_PRINT;
    strncpy(ins->result, arg, 63);
}

/* ----------------------------------------------------------
 * gen_code — recursively generate code for an expression.
 * Returns the operand (temp or variable name) holding the result.
 * The caller must copy the returned string before the next call.
 * ---------------------------------------------------------- */
char *gen_code(ASTNode *node) {
    static char buf[64];   /* small static buffer; sufficient here */

    if (!node) { buf[0] = '\0'; return buf; }

    switch (node->type) {

        case NODE_NUM:
            /* Return the literal as a string operand */
            sprintf(buf, "%d", node->num_val);
            return buf;

        case NODE_VAR:
            /* Return the variable name */
            strncpy(buf, node->name, 63);
            return buf;

        case NODE_BINOP: {
            /* Evaluate both sides, then combine in a new temp */
            char left_op[64], right_op[64], temp[64];

            strncpy(left_op,  gen_code(node->left),  63);
            strncpy(right_op, gen_code(node->right), 63);
            new_temp(temp);

            emit_binop(temp, left_op, node->op, right_op);
            strncpy(buf, temp, 63);
            return buf;
        }

        default:
            buf[0] = '\0';
            return buf;
    }
}

/* ----------------------------------------------------------
 * gen_statement — generate code for one statement node
 * ---------------------------------------------------------- */
void gen_statement(ASTNode *node) {
    if (!node) return;

    char expr_op[64];

    switch (node->type) {

        case NODE_DECL: {
            /* Evaluate initialiser, then assign to the variable */
            strncpy(expr_op, gen_code(node->left), 63);
            emit_assign(node->name, expr_op);
            break;
        }

        case NODE_ASSIGN: {
            strncpy(expr_op, gen_code(node->left), 63);
            emit_assign(node->name, expr_op);
            break;
        }

        case NODE_PRINT: {
            strncpy(expr_op, gen_code(node->left), 63);
            emit_print(expr_op);
            break;
        }

        default:
            break;
    }
}

/* ----------------------------------------------------------
 * generate_tac — walk the full statement list
 * ---------------------------------------------------------- */
void generate_tac(ASTNode *root) {
    ASTNode *stmt = (root && root->type == NODE_PROGRAM)
                    ? root->left
                    : root;
    while (stmt) {
        gen_statement(stmt);
        stmt = stmt->next;
    }
}

/* ----------------------------------------------------------
 * print_tac — display the generated TAC program
 * ---------------------------------------------------------- */
void print_tac(void) {
    printf("\n===== Three-Address Code (TAC) =====\n");
    for (int i = 0; i < tac_count; i++) {
        TACInstr *ins = &tac_program[i];
        switch (ins->type) {
            case TAC_ASSIGN:
                printf("  %s = %s\n", ins->result, ins->arg1);
                break;
            case TAC_BINOP:
                printf("  %s = %s %c %s\n",
                       ins->result, ins->arg1, ins->op, ins->arg2);
                break;
            case TAC_PRINT:
                printf("  print %s\n", ins->result);
                break;
        }
    }
    printf("=====================================\n\n");
}
