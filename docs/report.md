# Compiler Design Project Report

**Title:** Mini DSL Compiler using Lex, YACC, and C  
**Author:** Srevarshan  
**Register No:** RA2311026050039  
**Date:** April 2026  

---

## 1. Architecture Overview

The compiler follows the classic **front-end pipeline**:

```
Source Code (.dsl)
       │
       ▼
┌─────────────┐
│  Lexer      │  lexer.l   →  lex.yy.c
│  (Flex)     │
└──────┬──────┘
       │  token stream
       ▼
┌─────────────┐
│  Parser     │  parser.y  →  parser.tab.c / parser.tab.h
│  (Bison)    │
└──────┬──────┘
       │  builds
       ▼
┌─────────────┐
│  AST        │  ast.c / ast.h
└──────┬──────┘
       │  walks
       ▼
┌─────────────┐
│  Semantic   │  semantic.c / semantic.h
│  Analyser   │
└──────┬──────┘
       │  walks (if clean)
       ▼
┌─────────────┐
│  ICG (TAC)  │  icg.c / icg.h
└─────────────┘
       │
       ▼
Three-Address Code output
```

---

## 2. Module Explanations

### 2.1 Lexer (`lexer.l`)

- Written in Flex (Fast Lexical Analyser).
- Scans the source file character-by-character.
- Identifies and categorises **tokens**:

| Pattern | Token | Example |
|---------|-------|---------|
| `int` | `INT` | `int a` |
| `print` | `PRINT` | `print(c)` |
| `[a-zA-Z_][a-zA-Z0-9_]*` | `IDENTIFIER` | `myVar` |
| `[0-9]+` | `NUMBER` | `42` |
| `+`, `-`, `*`, `/` | `PLUS`, `MINUS`, `STAR`, `SLASH` | |
| `=` | `ASSIGN` | |
| `;`, `(`, `)` | `SEMICOLON`, `LPAREN`, `RPAREN` | |

- Prints each token as it is found (useful for debugging).
- Returns token codes to the Bison parser via `return TOKEN_NAME`.

### 2.2 Parser (`parser.y`)

- Written in Bison (GNU YACC).
- Implements an **LALR(1)** context-free grammar.
- Grammar rules (simplified):

```
program     → stmt_list
stmt_list   → stmt_list stmt | stmt
stmt        → INT IDENT = expr ;
            | print ( expr ) ;
expr        → expr + term | expr - term | term
term        → term * factor | term / factor | factor
factor      → NUMBER | IDENT | ( expr )
```

- **Semantic actions** in each rule construct AST nodes and link them
  into a statement chain using the `next` pointer.

### 2.3 AST (`ast.c` / `ast.h`)

- A tree built from `ASTNode` structs.
- Node types: `NODE_PROGRAM`, `NODE_DECL`, `NODE_PRINT`,
  `NODE_BINOP`, `NODE_NUM`, `NODE_VAR`.
- Key functions:

| Function | Purpose |
|----------|---------|
| `create_num_node(val)` | Leaf node for an integer literal |
| `create_var_node(name)` | Leaf node for a variable reference |
| `create_binop_node(op, l, r)` | Internal node for arithmetic |
| `create_decl_node(name, expr)` | Declaration statement |
| `create_print_node(expr)` | Print statement |
| `print_ast(node, indent)` | Recursive pretty-printer |
| `free_ast(node)` | Recursively frees all memory |

### 2.4 Semantic Analysis (`semantic.c` / `semantic.h`)

- Maintains a flat **symbol table** (array of `Symbol` structs).
- Two key checks performed on a single pass:

1. **Duplicate declaration** — if `int a` appears twice, an error is reported.
2. **Use before declaration** — if variable `z` is referenced before `int z = …`, an error is reported.

- Returns the total number of errors; if > 0, code generation is skipped.

### 2.5 Intermediate Code Generation (`icg.c` / `icg.h`)

- Generates **Three-Address Code (TAC)** — each instruction has at most
  one operator and three operands.
- Temporaries `t0`, `t1`, … are created automatically for sub-expressions.
- Instruction types: `TAC_ASSIGN`, `TAC_BINOP`, `TAC_PRINT`.

---

## 3. Workflow Step-by-Step

```
Step 1 — Lex
  Source text is read; the lexer emits a stream of tokens.

Step 2 — Parse
  Bison consumes tokens, matches grammar rules, and calls
  semantic actions that build AST nodes.

Step 3 — AST Print
  The tree is printed with indentation so the structure is clear.

Step 4 — Semantic Analysis
  The tree is walked; the symbol table is built and checked.
  Errors are reported and counted.

Step 5 — ICG
  (Only if semantic check passes)
  The tree is walked again; TAC instructions are emitted and printed.
```

---

## 4. Sample Run

### Input (`test/input1.txt`)

```
int a = 5;
int b = 10;
int c = a + b;
print(c);
```

### Output (abbreviated)

```
----- Phase 1: Lexical Analysis & Parsing -----
[TOKEN] KEYWORD : int
[TOKEN] IDENT   : a
...

----- Phase 2: Abstract Syntax Tree -----
[PROGRAM]
  [DECL] int a =
    [NUM] 5
  [DECL] int b =
    [NUM] 10
  [DECL] int c =
    [BINOP] '+'
      [VAR] a
      [VAR] b
  [PRINT]
    [VAR] c

----- Phase 3: Semantic Analysis -----
[SEMANTIC] No errors found. Program is valid.

----- Phase 4: Intermediate Code Generation -----
===== Three-Address Code (TAC) =====
  a = 5
  b = 10
  t0 = a + b
  c = t0
  print c
=====================================
```

### Error Input (`test/input2.txt`)

```
int x = 3;
int y = x * 2;
int x = 9;      ← duplicate declaration
print(z);       ← z undeclared
```

Expected errors:
```
[SEMANTIC ERROR] Variable 'x' already declared.
[SEMANTIC ERROR] Variable 'z' used before declaration.
[SEMANTIC] 2 error(s) detected.
[COMPILER] Semantic errors found. Aborting code generation.
```

---

## 5. Tools Used

| Tool | Version | Purpose |
|------|---------|---------|
| Flex | ≥ 2.6 | Lexical analysis |
| Bison | ≥ 3.0 | LALR(1) parsing |
| GCC | ≥ 9.0 | Compilation |
| Make (optional) | ≥ 4.0 | Build automation |

---

## 6. Conclusion

This project demonstrates a complete **front-end compiler pipeline** for a
custom DSL. Each phase is implemented as an independent, well-commented C
module, making it easy to extend (e.g., add `if`/`while` statements, a
back-end code generator, or an interpreter).
