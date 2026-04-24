/* ============================================================
 * lexer.l — Flex Lexical Analyser for the Mini DSL
 * Project: Mini DSL Compiler
 * Author : Srevarshan (RA2311026050039)
 *
 * Tokens produced:
 *   INT PRINT IDENTIFIER NUMBER
 *   PLUS MINUS STAR SLASH
 *   ASSIGN SEMICOLON LPAREN RPAREN
 * ============================================================ */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"   /* token definitions from Bison */
%}

/* Suppress default main and yywrap */
%option noyywrap

/* ----------------------------------------------------------
 * Pattern definitions
 * ---------------------------------------------------------- */
DIGIT   [0-9]
LETTER  [a-zA-Z_]
ID      {LETTER}({LETTER}|{DIGIT})*
NUMBER  {DIGIT}+

%%

    /* ---- Keywords ---- */
"int"       { printf("[TOKEN] KEYWORD : int\n");        return INT;       }
"print"     { printf("[TOKEN] KEYWORD : print\n");      return PRINT;     }

    /* ---- Identifiers ---- */
{ID}        { printf("[TOKEN] IDENT   : %s\n", yytext);
              yylval.str = strdup(yytext);
              return IDENTIFIER; }

    /* ---- Numbers ---- */
{NUMBER}    { printf("[TOKEN] NUMBER  : %s\n", yytext);
              yylval.num = atoi(yytext);
              return NUMBER; }

    /* ---- Operators ---- */
"+"         { printf("[TOKEN] OP      : +\n");          return PLUS;      }
"-"         { printf("[TOKEN] OP      : -\n");          return MINUS;     }
"*"         { printf("[TOKEN] OP      : *\n");          return STAR;      }
"/"         { printf("[TOKEN] OP      : /\n");          return SLASH;     }
"="         { printf("[TOKEN] OP      : =\n");          return ASSIGN;    }

    /* ---- Punctuation ---- */
";"         { printf("[TOKEN] PUNC    : ;\n");          return SEMICOLON; }
"("         { printf("[TOKEN] PUNC    : (\n");          return LPAREN;    }
")"         { printf("[TOKEN] PUNC    : )\n");          return RPAREN;    }

    /* ---- Whitespace and newlines (skip silently) ---- */
[ \t\r\n]+  { /* ignore */ }

    /* ---- Unknown character ---- */
.           { fprintf(stderr, "[LEX ERROR] Unknown character: '%s'\n", yytext); }

%%
