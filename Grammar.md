# SolvA Language Grammar

Complete grammar specification for the SolvA constraint logic programming language, from Program start state to EOF terminal state.

---

## 1. LEXICAL GRAMMAR (Tokens & Characters)

### 1.1 Keywords
```
KEYWORD_VAR        →  "var"
KEYWORD_IN         →  "in"
KEYWORD_CONSTRAINT →  "constraint"
KEYWORD_ABS        →  "abs"
KEYWORD_MANUAL     →  "Manual Inputs"
KEYWORD_MACHINE    →  "Machine Inputs"
```

### 1.2 Operators
```
OPERATOR → "=="  | "!="  | ">"  | "<"  | ">="  | "<="
```

### 1.3 Special Symbols (Single Characters)
```
ASSIGN    →  "="
SEMICOLON →  ";"
COLON     →  ":"
DOT       →  "."
LPAREN    →  "("
RPAREN    →  ")"
LBRACKET  →  "["
RBRACKET  →  "]"
MINUS     →  "-"
```

### 1.4 Identifiers & Literals
```
ID  →  [a-zA-Z_][a-zA-Z0-9_]*
INT →  [0-9]+
```

### 1.5 Comments
```
COMMENT → "//" [^\n]* <newline>
```

### 1.6 Whitespace & Newlines
```
WHITESPACE → [ \t\r]+ (ignored)
NEWLINE    → \n (counted for line tracking)
```

---

## 2. SYNTACTIC GRAMMAR (BNF Notation)

### 2.1 Program Structure

```
program
    → line EOF
```

### 2.2 Line & Statement Rules

```
line
    → statement line
    | input_section line
    | ε                          (empty - end of file)

statement
    → var_decl
    | constraint_def
    | assignment_stmt

input_section
    → KEYWORD_MANUAL ':'
    | KEYWORD_MACHINE ':'
```

### 2.3 Variable Declaration

```
var_decl
    → KEYWORD_VAR ID KEYWORD_IN '[' INT '..' INT ']' SEMICOLON

Example: var Sindh in [1..3];
         var Col1 in [1..4];
```

**Syntax Elements:**
- `KEYWORD_VAR` → "var" (literal keyword)
- `ID` → [a-zA-Z_][a-zA-Z0-9_]* (variable name)
- `KEYWORD_IN` → "in" (literal keyword)
- `'['` → "[" (literal character)
- `INT` → [0-9]+ (positive integer - minimum value)
- `'..'` → ".." (two DOT characters)
- `INT` → [0-9]+ (positive integer - maximum value)
- `']'` → "]" (literal character)
- `SEMICOLON` → ";" (statement terminator)

### 2.4 Constraint Definitions

#### 2.4.1 Binary Constraint (Variable OP Variable)

```
constraint_def
    → KEYWORD_CONSTRAINT ID OPERATOR ID SEMICOLON

Example: constraint Sindh != Punjab;
         constraint Col1 < Col2;
         constraint Alice > Bob;
```

**Syntax Elements:**
- `KEYWORD_CONSTRAINT` → "constraint" (literal keyword)
- `ID` → [a-zA-Z_][a-zA-Z0-9_]* (first variable)
- `OPERATOR` → "==" | "!=" | ">" | "<" | ">=" | "<=" (comparison operator)
- `ID` → [a-zA-Z_][a-zA-Z0-9_]* (second variable)
- `SEMICOLON` → ";" (statement terminator)

#### 2.4.2 Absolute Difference Constraint (|Var1 - Var2| OP Int)

```
constraint_def
    → KEYWORD_CONSTRAINT KEYWORD_ABS '(' ID '-' ID ')' OPERATOR INT SEMICOLON

Example: constraint abs(Col1 - Col2) != 1;
         constraint abs(Col3 - Col4) != 2;
```

**Syntax Elements:**
- `KEYWORD_CONSTRAINT` → "constraint" (literal keyword)
- `KEYWORD_ABS` → "abs" (literal keyword for absolute value function)
- `'('` → "(" (left parenthesis)
- `ID` → [a-zA-Z_][a-zA-Z0-9_]* (first variable)
- `'-'` → "-" (minus operator)
- `ID` → [a-zA-Z_][a-zA-Z0-9_]* (second variable)
- `')'` → ")" (right parenthesis)
- `OPERATOR` → "==" | "!=" | ">" | "<" | ">=" | "<=" (comparison operator)
- `INT` → [0-9]+ (integer constant)
- `SEMICOLON` → ";" (statement terminator)

### 2.5 Assignment Statements

```
assignment_stmt
    → ID ASSIGN INT SEMICOLON

Example: Col1 = 2;
         Alice = 4;
         Sindh = 1;
```

**Syntax Elements:**
- `ID` → [a-zA-Z_][a-zA-Z0-9_]* (variable name)
- `ASSIGN` → "=" (assignment operator)
- `INT` → [0-9]+ (value to assign)
- `SEMICOLON` → ";" (statement terminator)

---

## 3. TOKEN SUMMARY TABLE

| Token Name | Type | Pattern/Value | Example |
|-----------|------|---------------|---------|
| KEYWORD_VAR | Keyword | Literal | var |
| KEYWORD_IN | Keyword | Literal | in |
| KEYWORD_CONSTRAINT | Keyword | Literal | constraint |
| KEYWORD_ABS | Keyword | Literal | abs |
| KEYWORD_MANUAL | Keyword | Literal | Manual Inputs |
| KEYWORD_MACHINE | Keyword | Literal | Machine Inputs |
| OPERATOR | Token | One of 6 operators | !=, ==, <, >, <=, >= |
| ID | Token | [a-zA-Z_][a-zA-Z0-9_]* | Col1, Sindh, Alice |
| INT | Token | [0-9]+ | 1, 2, 100, 999 |
| ASSIGN | Symbol | = | = |
| SEMICOLON | Symbol | ; | ; |
| COLON | Symbol | : | : |
| DOT | Symbol | . | . |
| LPAREN | Symbol | ( | ( |
| RPAREN | Symbol | ) | ) |
| LBRACKET | Symbol | [ | [ |
| RBRACKET | Symbol | ] | ] |
| MINUS | Symbol | - | - |
| COMMENT | Ignored | //.*\n | // N-Queens Problem |
| WHITESPACE | Ignored | [ \t\r]+ | (space, tab, carriage return) |
| NEWLINE | Ignored | \n | (line feed) |
| EOF | Terminal | (end of input) | (implicit) |

---

## 4. CHARACTER MAPPING

### 4.1 All Characters Used in Grammar

| Character | Name | Usage |
|-----------|------|-------|
| `a-z` | Lowercase letters | ID start/continuation |
| `A-Z` | Uppercase letters | ID start/continuation |
| `0-9` | Digits | INT tokens, ID continuation |
| `_` | Underscore | ID start/continuation |
| `:` | Colon | Ends input sections (Manual Inputs:, Machine Inputs:) |
| `.` | Period | Range separator (..) |
| `,` | Comma | N/A (not used in grammar) |
| `-` | Hyphen/Minus | Subtraction in abs() constraints |
| `=` | Equals | Assignment (=), equality operator (==) |
| `!` | Exclamation | Not-equal operator (!=) |
| `<` | Less-than | Comparison operators (<, <=) |
| `>` | Greater-than | Comparison operators (>, >=) |
| `(` | Left paren | Groups abs() expression |
| `)` | Right paren | Closes abs() expression |
| `[` | Left bracket | Opens domain range |
| `]` | Right bracket | Closes domain range |
| `;` | Semicolon | Terminates statements |
| `/` | Forward slash | Comments (//...) |
| Space, Tab | Whitespace | Ignored (separates tokens) |
| Newline | Line break | Ignored (line counting) |

---

## 5. COMPLETE GRAMMAR RULE HIERARCHY

```
Program (Start Symbol)
  ├─ line (recursive list of statements)
  │   ├─ statement
  │   │   ├─ var_decl
  │   │   ├─ constraint_def
  │   │   │   ├─ Binary: ID OPERATOR ID
  │   │   │   └─ Absolute: abs(ID - ID) OPERATOR INT
  │   │   └─ assignment_stmt
  │   ├─ input_section
  │   │   ├─ Manual Inputs:
  │   │   └─ Machine Inputs:
  │   └─ ε (empty - EOF)
  └─ EOF (Terminal - end of input)
```

---

## 6. SEMANTIC CONSTRAINTS (Validation Rules)

These rules are checked during parsing (not purely syntactic):

```
1. Variable Declaration:
   - No duplicate variable names
   - min_value ≤ max_value
   
2. Constraint Definition:
   - Both variables must be previously declared
   - Operator must be valid: {==, !=, <, >, <=, >=}
   
3. Assignment Statement:
   - Variable must be previously declared
   - value must be in [min..max] domain
   - Variable cannot be assigned twice
   - Assignment must not violate existing constraints
   
4. Input Sections:
   - Manual Inputs: section (if present) must have all variables assigned
   - Machine Inputs: section for remaining unassigned variables
```

---

## 7. GRAMMAR EXAMPLES

### Example 1: N-Queens (4 Queens)
```
var Col1 in [1..4];
var Col2 in [1..4];
var Col3 in [1..4];
var Col4 in [1..4];

constraint Col1 != Col2;
constraint Col1 != Col3;
constraint Col1 != Col4;
constraint Col2 != Col3;
constraint Col2 != Col4;
constraint Col3 != Col4;

constraint abs(Col1 - Col2) != 1;
constraint abs(Col1 - Col3) != 2;
constraint abs(Col1 - Col4) != 3;
constraint abs(Col2 - Col3) != 1;
constraint abs(Col2 - Col4) != 2;
constraint abs(Col3 - Col4) != 1;

Manual Inputs:
Col1 = 2;
Col2 = 4;
Col3 = 1;
Col4 = 3;

Machine Inputs:
```

### Example 2: Friends Age Puzzle
```
var Alice in [1..4];
var Bob in [1..4];
var Carol in [1..4];
var David in [1..4];

constraint Alice != Bob;
constraint Alice != Carol;
constraint Alice != David;
constraint Bob != Carol;
constraint Bob != David;
constraint Carol != David;

constraint Alice > Bob;
constraint Carol < David;
constraint Bob > Carol;

Manual Inputs:
Alice = 4;
Bob = 2;
Carol = 1;
David = 3;

Machine Inputs:
```

---

## 8. OPERATOR PRECEDENCE & ASSOCIATIVITY

| Operator | Type | Precedence | Associativity | Meaning |
|----------|------|-----------|---------------|---------|
| `==` | Binary | Equal | N/A | Equality |
| `!=` | Binary | Equal | N/A | Not-equal |
| `<` | Binary | Equal | N/A | Less-than |
| `>` | Binary | Equal | N/A | Greater-than |
| `<=` | Binary | Equal | N/A | Less-than-or-equal |
| `>=` | Binary | Equal | N/A | Greater-than-or-equal |
| `abs()` | Unary | Highest | N/A | Absolute value function |
| `-` | Binary (inside abs) | Highest | Left | Subtraction |

---

## 9. GRAMMAR NOTES

1. **Left-Recursion**: The `line` rule uses left-recursion for efficiency
2. **Whitespace**: All whitespace is ignored during lexical analysis
3. **Comments**: Single-line comments only (// comment)
4. **No String Literals**: Identifiers are used instead of strings
5. **No Variable Scope**: All variables are global (flat namespace)
6. **Case Sensitivity**: All identifiers are case-sensitive
7. **Mandatory Sections**: Input sections are optional but recommended
8. **Line Tracking**: All errors include line numbers for debugging

---

## 10. EBNF NOTATION (Extended BNF)

### Compact Grammar Definition

```
Program    ::= Line EOF

Line       ::= (Statement | InputSection)* LineEnd

LineEnd    ::= EOF | ε

Statement  ::= VarDecl | ConstraintDef | AssignmentStmt

VarDecl    ::= "var" ID "in" "[" INT ".." INT "]" ";"

ConstraintDef ::= "constraint" (
                    (ID OPERATOR ID) |
                    ("abs" "(" ID "-" ID ")" OPERATOR INT)
                  ) ";"

AssignmentStmt ::= ID "=" INT ";"

InputSection ::= ("Manual Inputs" | "Machine Inputs") ":"

ID         ::= [a-zA-Z_][a-zA-Z0-9_]*

INT        ::= [0-9]+

OPERATOR   ::= "==" | "!=" | ">" | "<" | ">=" | "<="

Keyword    ::= "var" | "in" | "constraint" | "abs"
```

---

## Generated From
- `src/parser.y` - Yacc/Bison parser definition
- `src/lexer.l` - Flex lexical analyzer definition
- `src/parser.hpp` - Token definitions
- Complete validation includes semantic checking in `src/logic_engine.cpp` and `src/symbol_table.cpp`
