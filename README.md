# LispInterpreter

A complete interpreter for a subset of LISP, written in **C++20** with no external dependencies.

The interpreter processes source files through a classic **three-stage pipeline**: lexical analysis, syntax analysis via recursive descent parsing, and semantic evaluation using the **Visitor pattern** on an Abstract Syntax Tree (AST).

## Architecture

```
Source file (.txt) → Lexer → Token stream → Parser → AST → EvaluationVisitor → Output
```

### Lexer (`Lexer.h/.cpp`)

Scans the source file character by character and produces a `vector<Token>`. Recognizes parentheses, keywords (`BLOCK`, `SET`, `IF`, `WHILE`, `ADD`, `SUB`, etc.), integer constants (including negatives), and alphabetic identifiers. Rejects invalid characters and malformed numeric literals (e.g. leading zeros like `007`).

### Parser (`Parser.h/.cpp`)

Recursive descent parser that builds an AST from the token stream. The AST node hierarchy is split into three independent abstract base types defined in `Syntax.h`:

- **`Statement`** — `BlockStmt`, `SetStmt`, `InputStmt`, `PrintStmt`, `IfStmt`, `WhileStmt`
- **`NumExpr`** — `NumberNode`, `VariableNode`, `BinaryNumOpNode`
- **`BoolExpr`** — `BoolConstNode`, `RelOpNode`, `BoolBinaryOpNode`, `BoolNotNode`

This strict separation prevents the parser from confusing statements with expressions at the type level.

### Visitor (`Visitor.h`)

Abstract interface with a pure virtual `visit()` overload for every AST node. Each node implements `accept(Visitor&)` calling `visitor.visit(*this)`, enabling **double dispatch** at runtime. Two concrete visitors are provided:

- **`EvaluationVisitor`** — Executes the program. Uses two accumulator registers (`int64_t` for numeric results, `bool` for boolean results) to propagate values across recursive visits. Implements **short-circuit evaluation** for `AND`/`OR`.
- **`PrintVisitor`** — Pretty-prints the AST back in LISP notation (useful for debugging the parser).

### Symbol Table (`SymbolTable.h`)

Stores global variables as `unordered_map<string, int64_t>`. Throws on access to undeclared variables.

### Error Handling (`Exception.h`)

Three custom exception types derived from `std::runtime_error`:

- `LexicalError` — invalid characters or malformed constants
- `SyntaxError` — grammar violations, mismatched parentheses
- `EvaluationError` — runtime errors (division by zero, undeclared variables)

### Memory Management

AST nodes use **raw pointers** with manual recursive deallocation in destructors — each parent node deletes its children, cascading down the tree. The parser uses `try-catch` blocks to clean up partially built subtrees on exceptions, ensuring no memory leaks.

## Grammar

```
program     → stmt_block
stmt_block  → statement | ( BLOCK statement+ )
statement   → ( SET ID num_expr )
            | ( INPUT ID )
            | ( PRINT num_expr )
            | ( IF bool_expr stmt_block stmt_block )
            | ( WHILE bool_expr stmt_block )
num_expr    → CONST | ID | ( ADD|SUB|MUL|DIV num_expr num_expr )
bool_expr   → TRUE | FALSE
            | ( LT|GT|EQ num_expr num_expr )
            | ( AND|OR bool_expr bool_expr )
            | ( NOT bool_expr )
```

## Build

```bash
g++ -std=c++17 -O2 -o lispInterpreter lispInterpreter.cpp Lexer.cpp Parser.cpp Syntax.cpp Token.cpp
```

## Usage

```bash
./lispInterpreter <source_file.txt>
```

## Examples

**Factorial (n=10):**
```lisp
(BLOCK
  (SET n 10)
  (SET result 1)
  (WHILE (GT n 0)
    (BLOCK
      (SET result (MUL result n))
      (SET n (SUB n 1))))
  (PRINT result))
```
Output: `3628800`

**Simple expression:**
```lisp
(PRINT (MUL 5 4))
```
Output: `20`

## Tests

The `LispInterpreter_TestFiles/` directory contains 34 test cases (`.txt` input + `.out` expected output):

- **18 PASS tests** — correct programs (factorial, fibonacci, primes, logarithm, boolean expressions, short-circuit evaluation, etc.)
- **16 FAIL tests** — error cases (empty program, division by zero, mismatched parentheses, undeclared variables, lexical errors, missing operands, etc.)

## Project Structure

```
├── lispInterpreter.cpp     # main — orchestrates the 3 phases
├── Token.h / Token.cpp     # Token definition and keyword map
├── Lexer.h / Lexer.cpp     # Lexical analyzer
├── Syntax.h / Syntax.cpp   # AST node hierarchy + accept()
├── Parser.h / Parser.cpp   # Recursive descent parser
├── Visitor.h               # Abstract Visitor interface
├── EvaluationVisitor.h     # Semantic evaluator
├── PrintVisitor.h          # AST pretty-printer
├── SymbolTable.h           # Variable storage
├── Exception.h             # Custom exceptions
├── UML_LISP_INTERPRETE.pdf # UML class diagram
└── LispInterpreter_TestFiles/
    ├── PASS_*.txt / .out   # Correct execution tests
    └── FAIL_*.txt / .out   # Error handling tests
```
