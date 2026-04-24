/* ============================================================
 * parser.y — Bison Parser for the Mini DSL
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 *
 * Grammar (BNF):
 *   program     : stmt_list
 *   stmt_list   : stmt_list stmt | stmt
 *   stmt        : decl_stmt | print_stmt
 *   decl_stmt   : INT IDENTIFIER ASSIGN expr SEMICOLON
 *   print_stmt  : PRINT LPAREN expr RPAREN SEMICOLON
 *   expr        : expr PLUS  term
 *               | expr MINUS term
 *               | term
 *   term        : term STAR  factor
 *               | term SLASH factor
 *               | factor
 *   factor      : NUMBER | IDENTIFIER | LPAREN expr RPAREN
 * ============================================================ */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"
#include "icg.h"

/* Root of the AST — filled by the parser, used in main.c */
ASTNode *ast_root = NULL;

/* Prototype for the lexer */
int yylex(void);

/* Error handler */
void yyerror(const char *msg) {
    fprintf(stderr, "[PARSE ERROR] %s\n", msg);
}
%}

/* ----------------------------------------------------------
 * Value union
 * ---------------------------------------------------------- */
%union {
    int       num;   /* for NUMBER tokens              */
    char     *str;   /* for IDENTIFIER tokens          */
    ASTNode  *node;  /* for AST sub-trees              */
}

/* ----------------------------------------------------------
 * Token declarations
 * ---------------------------------------------------------- */
%token <num> NUMBER
%token <str> IDENTIFIER
%token INT PRINT
%token ASSIGN SEMICOLON LPAREN RPAREN
%token PLUS MINUS STAR SLASH

/* ----------------------------------------------------------
 * Type declarations for non-terminals
 * ---------------------------------------------------------- */
%type <node> program stmt_list stmt decl_stmt print_stmt
%type <node> expr term factor

/* ----------------------------------------------------------
 * Operator precedence (lowest → highest)
 * ---------------------------------------------------------- */
%left PLUS MINUS
%left STAR SLASH

%%

/* ----------------------------------------------------------
 * Grammar rules
 * ---------------------------------------------------------- */

/* Entry point */
program
    : stmt_list
        {
            /* Wrap in a PROGRAM node and save as root */
            ASTNode *prog = (ASTNode *)calloc(1, sizeof(ASTNode));
            prog->type    = NODE_PROGRAM;
            prog->left    = $1;
            ast_root      = prog;
        }
    ;

/* Statement list: build a linked chain via ->next */
stmt_list
    : stmt_list stmt
        {
            /* Append $2 at the end of the list starting at $1 */
            ASTNode *cur = $1;
            while (cur->next) cur = cur->next;
            cur->next = $2;
            $$ = $1;
        }
    | stmt
        { $$ = $1; }
    ;

/* A single statement */
stmt
    : decl_stmt  { $$ = $1; }
    | print_stmt { $$ = $1; }
    ;

/* int identifier = expr ; */
decl_stmt
    : INT IDENTIFIER ASSIGN expr SEMICOLON
        {
            $$ = create_decl_node($2, $4);
            free($2);          /* strdup'd by lexer */
        }
    ;

/* print ( expr ) ; */
print_stmt
    : PRINT LPAREN expr RPAREN SEMICOLON
        {
            $$ = create_print_node($3);
        }
    ;

/* Additive expressions */
expr
    : expr PLUS  term   { $$ = create_binop_node('+', $1, $3); }
    | expr MINUS term   { $$ = create_binop_node('-', $1, $3); }
    | term              { $$ = $1; }
    ;

/* Multiplicative expressions */
term
    : term STAR  factor { $$ = create_binop_node('*', $1, $3); }
    | term SLASH factor { $$ = create_binop_node('/', $1, $3); }
    | factor            { $$ = $1; }
    ;

/* Atoms */
factor
    : NUMBER                     { $$ = create_num_node($1); }
    | IDENTIFIER                 { $$ = create_var_node($1); free($1); }
    | LPAREN expr RPAREN         { $$ = $2; }
    ;

%%
