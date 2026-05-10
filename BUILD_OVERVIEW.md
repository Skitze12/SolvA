# SolvA Compiler - File Overview & Error Handling Phases

## PART 1: FILE OVERVIEW (src folder)

### 1. **lexer.l** (Flex Lexical Analyzer)
**What it does:**
- Tokenizes the input `.solva` file into tokens
- Scans character-by-character and matches against patterns
- Recognizes keywords, operators, identifiers, integers, symbols

**Compilation:**
```bash
flex -o src/lex.yy.c src/lexer.l
# Produces: src/lex.yy.c (C code for lexical analyzer)
```

**Output:**
- `src/lex.yy.c` - C implementation of the scanner
- Provides the `yylex()` function that returns tokens to the parser

**Processes:**
```
Input File (sudoku.solva)
    ↓
[LEXER scans text]
    ↓
Output: Token Stream
  Examples: KEYWORD_VAR, ID, KEYWORD_IN, INT, INT, SEMICOLON, ...
```

---

### 2. **parser.y** (Bison Parser)
**What it does:**
- Defines grammar rules in YACC format
- Uses tokens from lexer to build Abstract Syntax Tree (AST)
- Performs syntactic and semantic validation
- Calls logic engine and symbol table for constraint checking

**Compilation:**
```bash
bison -d -o src/parser.cpp src/parser.y
# Produces: 
#   src/parser.cpp (C++ parser implementation)
#   src/parser.hpp (header with token definitions)
```

**Output:**
- `src/parser.cpp` - Parser implementation
- `src/parser.hpp` - Token definitions for lexer to use
- Implements `yyparse()` function

**Processes:**
```
Token Stream (from lexer)
    ↓
[PARSER validates grammar rules]
    ↓
Output: AST (Abstract Syntax Tree)
```

---

### 3. **ast.cpp / ast.h** (Abstract Syntax Tree)
**What it does:**
- Defines node classes for AST representation
- Each node type represents a language construct (VarDeclNode, ConstraintNode, etc.)
- Generates DOT format output for visualization
- Maintains tree hierarchy and connections

**Compilation (as object file):**
```bash
g++ -std=c++11 -I./include -c -o src/ast.o src/ast.cpp
```

**Output:**
- `src/ast.o` - Object file
- AST structure in memory during parsing

**Node Types:**
- `VarDeclNode` - Variable declarations
- `ConstraintNode` - Constraints (var OP var)
- `AbsConstraintNode` - Absolute difference constraints
- `AssignmentNode` - Variable assignments
- `LineNode` - Program lines
- `EOFNode` - End of file marker
- `TerminalNode` - Lexical symbols

**Processes:**
```
Parser builds AST nodes
    ↓
[ast.cpp stores node data]
    ↓
Output: Tree structure + DOT visualization format
```

---

### 4. **symbol_table.cpp / symbol_table.h** (Symbol Table)
**What it does:**
- Maintains a registry of all declared variables
- Stores variable domains (min..max bounds)
- Tracks assigned values during parsing
- Validates domain membership
- Detects duplicate declarations

**Compilation:**
```bash
g++ -std=c++11 -I./include -c -o src/symbol_table.o src/symbol_table.cpp
```

**Output:**
- `src/symbol_table.o` - Object file

**Functions:**
- `registerVariable(name, min, max)` - Add variable to registry
- `setValue(name, value)` - Assign value to variable
- `hasValue(name)` - Check if variable is assigned
- `getValue(name)` - Retrieve variable value
- `isValueInDomain(name, value)` - Verify value in [min..max]
- `variableExists(name)` - Check declaration

**Processes:**
```
Parser declares: var Col1 in [1..4];
    ↓
[Symbol Table registers: Col1 → [1, 4]]
    ↓

Parser assigns: Col1 = 2;
    ↓
[Symbol Table validates: 2 ∈ [1, 4] ✓]
[Symbol Table stores: Col1 = 2]
```

---

### 5. **logic_engine.cpp / logic_engine.h** (Constraint Logic Engine)
**What it does:**
- Stores all constraint rules from the program
- Validates assignments against constraints (no violations)
- Performs constraint satisfaction solving with backtracking
- Tests if a value assignment satisfies all constraints

**Compilation:**
```bash
g++ -std=c++11 -I./include -c -o src/logic_engine.o src/logic_engine.cpp
```

**Output:**
- `src/logic_engine.o` - Object file

**Functions:**
- `addRule(var1, op, var2)` - Add binary constraint
- `addRule(var1, var2, op, int)` - Add absolute difference constraint
- `isMoveValid(var, value, symbolTable)` - Check if assignment violates constraints
- `findSolution(symbolTable)` - Solve for remaining unassigned variables

**Constraint Evaluation Logic:**
```
Rule: constraint Col1 != Col2;
When Col1 = 2:
    ↓
Check: Does 2 != Col2 (if Col2 assigned)?
    ↓
    If Col2=3: Satisfied ✓
    If Col2=2: Violated ✗

Rule: constraint abs(Col1 - Col2) != 1;
When Col1 = 1, Col2 = 2:
    ↓
Calculate: |1 - 2| = 1
Check: Is 1 != 1? NO ✗ VIOLATION
```

**Processes:**
```
Parser stores constraint
    ↓
[Logic Engine indexes all rules]
    ↓

When variable assigned (Col1 = 2):
    ↓
[Check: Does this violate any rule?]
    → Compare with already-assigned variables
    → Check absolute differences if applicable
    ↓
    If valid: Accept assignment
    If invalid: ERROR (Constraint Violation)
```

---

### 6. **visualizer.cpp / visualizer.h** (Visualization)
**What it does:**
- Converts AST into DOT language (Graphviz format)
- Generates tree diagram representation
- Can be converted to PNG, SVG, PDF using Graphviz tools
- Helps visualize parse tree structure

**Compilation:**
```bash
g++ -std=c++11 -I./include -c -o src/visualizer.o src/visualizer.cpp
```

**Output:**
- `src/visualizer.o` - Object file
- Generates `output.dot` file (DOT format)

**Processes:**
```
AST structure
    ↓
[Visualizer traverses AST]
    ↓
Output: output.dot (Graphviz format)
    ↓
[External tool converts to PNG/SVG]
    dot -Tpng output.dot -o output.png
```

---

### 7. **main.cpp** (Entry Point)
**What it does:**
- Program entry point
- Reads `.solva` input file
- Coordinates lexer, parser, symbol table, logic engine
- Outputs results and visualization
- Handles error reporting

**Compilation:**
```bash
g++ -std=c++11 -I./include -c -o src/main.o src/main.cpp
```

**Output:**
- `src/main.o` - Object file

**Main Workflow:**
```
main.cpp
    ↓
1. Validate command line arguments
2. Open input file (sudoku.solva)
3. Set yyin = file pointer
4. Call yyparse() → triggers lexer + parser
5. Access astRoot (built by parser)
6. Call visualizer to generate output.dot
7. Print results to console
8. Clean up and close file
```

---

## PART 2: COMPILATION PROCESS (Step-by-Step)

### Command-by-Command Compilation:

```bash
# Step 1: Generate lexer from Flex specification
flex -o src/lex.yy.c src/lexer.l

# Step 2: Generate parser from Bison specification
bison -d -o src/parser.cpp src/parser.y

# Step 3: Compile lexer to object file
g++ -std=c++11 -I./include -c -o src/lex.yy.o src/lex.yy.c

# Step 4: Compile parser to object file
g++ -std=c++11 -I./include -c -o src/parser.o src/parser.cpp

# Step 5: Compile AST
g++ -std=c++11 -I./include -c -o src/ast.o src/ast.cpp

# Step 6: Compile Symbol Table
g++ -std=c++11 -I./include -c -o src/symbol_table.o src/symbol_table.cpp

# Step 7: Compile Logic Engine
g++ -std=c++11 -I./include -c -o src/logic_engine.o src/logic_engine.cpp

# Step 8: Compile Visualizer
g++ -std=c++11 -I./include -c -o src/visualizer.o src/visualizer.cpp

# Step 9: Compile Main
g++ -std=c++11 -I./include -c -o src/main.o src/main.cpp

# Step 10: Link all object files into executable
g++ -std=c++11 -o solva src/main.o src/ast.o src/logic_engine.o src/symbol_table.o \
    src/visualizer.o src/lex.yy.o src/parser.o
```

### Dependency Graph:

```
lexer.l  ──[flex]──→  lex.yy.c  ──[compile]──→  lex.yy.o
                                                    ↓
parser.y ──[bison]──→ parser.cpp ──[compile]──→  parser.o
                      parser.hpp (header)           ↓
                          ↑                          ↓
                          └─ Used by lexer  ────────┤
                                                    ↓
ast.cpp ─────────────────────────────────→ ast.o ─→│
symbol_table.cpp ─────────────────────→ symbol_table.o ──→│
logic_engine.cpp ──────────────────────→ logic_engine.o ──→│ LINK → solva
visualizer.cpp ────────────────────────→ visualizer.o ────→│
main.cpp ──────────────────────────────→ main.o ───────→│
```

---

## PART 3: COMPILATION ERROR PHASES & HANDLING

### PHASE 1: LEXICAL ANALYSIS (lexer.l)

**What happens:**
- Character-by-character scanning
- Pattern matching against defined tokens
- Unrecognized characters cause errors

**Errors Detected:**
```
Error Type              Example                     Resolution
─────────────────────────────────────────────────────────────────────
Invalid characters     "string" or @ or #          Syntax error message
Unknown symbols        ` or ~ or &                 Lexical error with line number
Malformed numbers      123abc456                   Token mismatch
```

**Example Error Output:**
```
LEXICAL ERROR at line 5: Unrecognized character '"'.
Strings are not supported. Use identifiers instead.
```

**How it handles:**
- Checks character against regex patterns
- If no match: prints error with line number
- Fails gracefully without crashing

---

### PHASE 2: SYNTACTIC ANALYSIS (parser.y - Bison)

**What happens:**
- Token stream validation against grammar rules
- Ensures tokens follow proper structure
- Builds AST if grammar is valid

**Errors Detected:**
```
Error Type                    Example                        How Detected
──────────────────────────────────────────────────────────────────────
Missing semicolon             var X in [1..5]               Expected ; not found
Missing keyword               X in [1..5];                  Expected "var" keyword
Wrong symbol order            var in X [1..5];              Unexpected token
Invalid bracket nesting       var X in 1..5];               Bracket mismatch
Wrong operator format         constraint X = Y;             Wrong operator
Incomplete constraint         constraint X !=;              Missing second var
```

**Example Error Output:**
```
SYNTAX ERROR at line 3: syntax error
Common syntax issues:
  - Missing or misplaced semicolon (;)
  - Invalid syntax in variable declaration: var NAME in [MIN..MAX];
  - Invalid syntax in constraint: constraint VAR1 OP VAR2;
```

**How it handles:**
- Parser (Bison) expects specific token sequences
- If unexpected token found: calls `yyerror()`
- Terminates with YYERROR macro
- Print helpful error message with line number

---

### PHASE 3: SEMANTIC ANALYSIS (Parser + Symbol Table + Logic Engine)

**What happens:**
- Variable declaration validation
- Type/domain checking
- Constraint satisfaction testing
- Reference resolution

**Errors Detected:**

#### 3a. Variable Declaration Errors:
```
Error Type                      Example                          Check Location
─────────────────────────────────────────────────────────────────────────────
Duplicate variable name         var X in [1..5];                Symbol Table
                                var X in [6..10];               (variableExists)

Invalid domain                  var X in [5..2];                Parser rule
(min > max)                                                      (5 > 2 check)

Unassigned in Manual Inputs     Manual Inputs:                  Parser post-parse
                                X = 1;  (Y not assigned)        (hasManualInputsSection check)
```

**Example Error Output:**
```
SEMANTIC ERROR at line 3: Variable 'Col1' has already been declared!
```

```
SEMANTIC ERROR at line 5: Invalid domain for 'X': 
min value (5) cannot be greater than max value (2)
```

```
SEMANTIC ERROR: Manual Inputs section found but the following variables 
are not assigned:
  - Y
```

#### 3b. Constraint Reference Errors:
```
Error Type                          Example                     Check Location
─────────────────────────────────────────────────────────────────────────
Undefined variable in constraint    constraint X != Y;          Parser + Symbol Table
(Y never declared)                  (but Y not declared)        (variableExists check)
```

**Example Error Output:**
```
SEMANTIC ERROR at line 7: Variable 'Y' used in constraint 
but never declared!
```

#### 3c. Assignment Errors:
```
Error Type                          Example                     Check Location
──────────────────────────────────────────────────────────────
Variable not declared              X = 5;                      Symbol Table
                                   (X never declared)          (variableExists)

Value out of domain                var X in [1..4];            Symbol Table
                                   X = 10;  (10 not in [1..4]) (isValueInDomain)

Duplicate assignment               X = 1;                      Symbol Table
                                   X = 2;  (assigned twice)    (hasValue check)
```

**Example Error Output:**
```
VALUE ERROR at line 10: Cannot assign X = 10 
(value out of domain [1..4])
```

```
SEMANTIC ERROR at line 12: Variable 'X' has already 
been assigned a value!
```

#### 3d. Constraint Violation Errors:
```
Error Type                          Example                     Check Location
──────────────────────────────────────────────────────────────
Violates inequality                constraint X != Y;           Logic Engine
                                   X = 5; Y = 5;               (isMoveValid)

Violates comparison                constraint X > Y;           Logic Engine
                                   X = 2; Y = 5;               (operator check)

Violates abs difference            constraint abs(X-Y) != 1;   Logic Engine
                                   X = 1; Y = 2;               (|1-2| = 1 ✗)
```

**Example Error Output:**
```
CONSTRAINT VIOLATION at line 15: Cannot assign Col1 = 2 
(violates existing constraints)
```

---

## PART 4: ERROR HANDLING FLOW DIAGRAM

```
Input File (sudoku.solva)
    ↓
┌───────────────────────────────────┐
│ PHASE 1: LEXICAL ANALYSIS         │  File: lexer.l
│ (Character scanning)              │  Function: yylex()
└───────────────────────────────────┘
    ↓
    Errors? → LEXICAL ERROR → Exit with error code 1
    ↓
Token Stream
    ↓
┌───────────────────────────────────┐
│ PHASE 2: SYNTACTIC ANALYSIS       │  File: parser.y
│ (Grammar validation)              │  Function: yyparse()
└───────────────────────────────────┘
    ↓
    Errors? → SYNTAX ERROR → yyerror() → Exit with YYERROR
    ↓
AST + Symbol Table Population
    ↓
┌───────────────────────────────────┐
│ PHASE 3: SEMANTIC ANALYSIS        │  Files: parser.y, symbol_table.cpp
│ (Variable/Constraint validation)  │         logic_engine.cpp
│                                   │
│ 3a. Variable Validation           │  Check: Duplicate declaration?
│                                   │         Valid domain (min≤max)?
│                                   │  
│ 3b. Reference Resolution          │  Check: Var exists before use?
│                                   │
│ 3c. Assignment Validation         │  Check: Var declared?
│                                   │         Value in domain?
│                                   │         Var not assigned twice?
│                                   │
│ 3d. Constraint Satisfaction       │  Check: Assignment violates rule?
└───────────────────────────────────┘
    ↓
    Errors? → SEMANTIC ERROR / CONSTRAINT VIOLATION → Exit
    ↓
Valid Program
    ↓
┌───────────────────────────────────┐
│ PHASE 4: SOLUTION GENERATION      │  File: logic_engine.cpp
│ (Machine input solving)           │  Function: findSolution()
│                                   │
│ Find values for unassigned vars   │  Uses: Backtracking algorithm
│ using constraint satisfaction     │
└───────────────────────────────────┘
    ↓
Output Results
    ├─ Console output (variable values)
    ├─ AST visualization (output.dot)
    └─ Success exit code (0)
```

---

## PART 5: ERROR CHECKING LOCATIONS IN SOURCE CODE

| Phase | File | Function | Error Check |
|-------|------|----------|-------------|
| LEXICAL | lexer.l | (.) rule | Invalid character |
| SYNTACTIC | parser.y | var_decl rule | Token sequence validation |
| SEMANTIC | parser.y | var_decl rule | Duplicate var, invalid domain |
| SEMANTIC | parser.y | constraint_def rule | Variable existence check |
| SEMANTIC | parser.y | assignment_stmt rule | Var existence, domain check, constraint validation |
| SEMANTIC | symbol_table.cpp | registerVariable() | Duplicate prevention |
| SEMANTIC | symbol_table.cpp | isValueInDomain() | Domain membership test |
| SEMANTIC | logic_engine.cpp | isMoveValid() | Constraint satisfaction check |
| SEMANTIC | logic_engine.cpp | findSolution() | Backtracking constraint solver |

---

## PART 6: COMPILATION COMMAND SUMMARY

**Manual Compilation (Step-by-Step):**
```bash
flex -o src/lex.yy.c src/lexer.l
bison -d -o src/parser.cpp src/parser.y
g++ -std=c++11 -I./include -c -o src/lex.yy.o src/lex.yy.c
g++ -std=c++11 -I./include -c -o src/parser.o src/parser.cpp
g++ -std=c++11 -I./include -c -o src/ast.o src/ast.cpp
g++ -std=c++11 -I./include -c -o src/symbol_table.o src/symbol_table.cpp
g++ -std=c++11 -I./include -c -o src/logic_engine.o src/logic_engine.cpp
g++ -std=c++11 -I./include -c -o src/visualizer.o src/visualizer.cpp
g++ -std=c++11 -I./include -c -o src/main.o src/main.cpp
g++ -std=c++11 -o solva src/main.o src/ast.o src/logic_engine.o \
    src/symbol_table.o src/visualizer.o src/lex.yy.o src/parser.o
```

**Automated (using Makefile):**
```bash
make           # Compile everything
make clean     # Remove generated files
make run       # Run the executable
```
