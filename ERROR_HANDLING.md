# SolvA Compiler - Error Types & Handling Locations

Based on your explanation of the compilation pipeline:

```
Input (.solva) → [LEXER.l] → [PARSER.y] → [AST] → [SYMBOL_TABLE] → [LOGIC_ENGINE] → Output
                   ↓
               yylex()
              (tokens)
```

---

## 1. LEXICAL ERRORS (lexer.l)

**Location:** `src/lexer.l` - The `.` rule at the end (catch-all for unrecognized characters)

**What it checks:**
- Character-by-character scanning using regex patterns
- Any character that doesn't match defined patterns is INVALID
- Returns error immediately when found

**Examples of Lexical Errors:**

| Error | Input | Why Invalid | Line |
|-------|-------|-----------|------|
| String quotes | `"hello"` | Strings not supported | lexer.l line 47-52 |
| Invalid symbol @ | `var@X in [1..5]` | @ is not a valid character | lexer.l line 47-52 |
| Invalid symbol # | `#comment` (without //) | # is not valid | lexer.l line 47-52 |
| Backtick | `` `identifier` `` | Not in valid ID pattern | lexer.l line 47-52 |

**Error Output:**
```
LEXICAL ERROR at line 5: Unrecognized character '"'.
Strings are not supported. Use identifiers instead.

LEXICAL ERROR at line 3: Invalid character '@'. 
Comments start with //.
```

**How lexer.l handles it:**
```flex
.                       { 
    // Catch-all rule for unrecognized characters
    if (yytext[0] == '"' || yytext[0] == '\'') {
        std::cerr << "LEXICAL ERROR at line " << yylineno 
                  << ": Strings are not supported..." << std::endl;
    } else if (yytext[0] == '#' || yytext[0] == '@') {
        std::cerr << "LEXICAL ERROR at line " << yylineno 
                  << ": Invalid character '" << yytext[0] << "'..." << std::endl;
    } else {
        std::cerr << "LEXICAL ERROR at line " << yylineno 
                  << ": Unrecognized character '" << yytext[0] << "'." << std::endl;
    }
    return 1;  // Signal error
}
```

**Valid Patterns (lexer.l accepts):**
```
KEYWORD_VAR        → "var"
KEYWORD_IN         → "in"
KEYWORD_CONSTRAINT → "constraint"
KEYWORD_ABS        → "abs"
OPERATOR           → "==" | "!=" | ">" | "<" | ">=" | "<="
ID                 → [a-zA-Z_][a-zA-Z0-9_]*
INT                → [0-9]+
SYMBOLS            → "=" ";" ":" "." "[" "]" "(" ")" "-"
COMMENT            → "//" [anything]
```

---

## 2. SYNTAX ERRORS (parser.y)

**Location:** `src/parser.y` - Grammar rules section + `yyerror()` function

**What it checks:**
- Token sequence validation against grammar rules
- Expected specific patterns (var ID in [ INT .. INT ] ;)
- Bison matches tokens to rules; if no match → SYNTAX ERROR

**Examples of Syntax Errors:**

| Error | Input | Why Invalid | Where Detected |
|-------|-------|-----------|-----------------|
| Missing semicolon | `var Col1 in [1..4]` | Expected `;` at end | parser.y var_decl rule |
| Missing keyword | `Col1 in [1..4];` | Expected `var` keyword | parser.y program rule |
| Wrong bracket | `var Col1 in (1..4);` | Expected `[`, got `(` | parser.y var_decl rule |
| Wrong dots | `var Col1 in [1.4];` | Expected `..`, got `.` | parser.y var_decl rule |
| Missing colon after section | `Manual Inputs` (no `:`) | Expected `:` after | parser.y input_section rule |
| Wrong constraint syntax | `constraint Col1 Col2 !=;` | Expected `ID OP ID` order | parser.y constraint_def rule |
| Missing closing bracket | `var X in [1..5;` | Expected `]` | parser.y var_decl rule |

**Grammar Rules (parser.y):**

```yacc
// CORRECT SYNTAX
var_decl : KEYWORD_VAR ID KEYWORD_IN '[' INT '.' '.' INT ']' SEMICOLON
         {
             // Process valid declaration
         }

// If parser encounters different token sequence:
// var Col1 in [1..4]  ← Missing SEMICOLON
// Parser expects: ... INT ']' SEMICOLON
// Gets: ... INT ']' EOF or next line
// Result: SYNTAX ERROR
```

**Error Output:**
```
SYNTAX ERROR at line 3: syntax error

Common syntax issues:
  - Missing or misplaced semicolon (;)
  - Invalid syntax in variable declaration: var NAME in [MIN..MAX];
  - Invalid syntax in constraint: constraint VAR1 OP VAR2;
  - Invalid syntax in assignment: VAR = VALUE;
  - Missing colon (:) after 'Manual Inputs' or 'Machine Inputs'
```

**How parser.y handles it:**

```yacc
// At end of parser.y
void yyerror(const char* s) {
    std::cerr << "SYNTAX ERROR at line " << yylineno << ": " << s << std::endl;
    std::cerr << "\nCommon syntax issues:" << std::endl;
    std::cerr << "  - Missing or misplaced semicolon (;)" << std::endl;
    std::cerr << "  - Invalid syntax in variable declaration: "
              << "var NAME in [MIN..MAX];" << std::endl;
    // ... more helpful messages
}

// Triggered when parser can't match token sequence to any grammar rule
```

**Bison Error Handling:**
```yacc
program : line
        {
            // If parsing fails at any point,
            // Bison calls yyerror()
            // Stops parsing
            // Returns error status
        }
        ;
```

---

## 3. SEMANTIC ERRORS (parser.y + symbol_table.cpp + logic_engine.cpp)

**Locations:**
1. `src/parser.y` - Variable declaration and assignment rules
2. `src/symbol_table.cpp` - Variable registry functions
3. `src/logic_engine.cpp` - Constraint validation functions

**What they check:**
- Variable meaning and validity (declared before use?)
- Type/domain correctness (value in range?)
- Constraint consistency (rules not violated?)
- Uniqueness (no duplicate declarations?)

---

### 3A. DUPLICATE VARIABLE DECLARATION ERROR

**Location:** `src/parser.y` (var_decl rule, lines ~120-135)

**What it checks:**
Before registering a new variable, check if already exists

**Example:**
```solva
var Col1 in [1..4];
var Col1 in [1..6];  ← ERROR: Col1 already declared!
```

**Error Code (parser.y var_decl rule):**
```cpp
var_decl : KEYWORD_VAR ID KEYWORD_IN '[' INT '.' '.' INT ']' SEMICOLON
         {
             // ❌ ERROR CHECK: Duplicate declaration
             if (symbolTable.variableExists($2)) {
                 std::cerr << "SEMANTIC ERROR at line " << yylineno 
                           << ": Variable '" << $2 
                           << "' has already been declared!" << std::endl;
                 YYERROR;  // Stop parsing
             }
             
             // ✓ If not duplicate, register it
             symbolTable.registerVariable($2, $5, $8);
         }
```

**Error Output:**
```
SEMANTIC ERROR at line 4: Variable 'Col1' has already been declared!
```

**Handled by:**
```cpp
// src/symbol_table.cpp
bool SymbolTable::variableExists(const std::string& name) const {
    return domains.find(name) != domains.end();
}
```

---

### 3B. INVALID DOMAIN ERROR (min > max)

**Location:** `src/parser.y` (var_decl rule, lines ~136-142)

**What it checks:**
Domain must satisfy: min_value ≤ max_value

**Example:**
```solva
var Col1 in [5..2];  ← ERROR: 5 > 2, invalid domain!
```

**Error Code (parser.y var_decl rule):**
```cpp
var_decl : KEYWORD_VAR ID KEYWORD_IN '[' INT '.' '.' INT ']' SEMICOLON
         {
             // ... duplicate check ...
             
             // ❌ ERROR CHECK: Invalid domain
             if ($5 > $8) {  // $5 = min value, $8 = max value
                 std::cerr << "SEMANTIC ERROR at line " << yylineno 
                           << ": Invalid domain for '" << $2 
                           << "': min value (" << $5 
                           << ") cannot be greater than max value (" << $8 
                           << ")" << std::endl;
                 YYERROR;  // Stop parsing
             }
             
             // ✓ If domain valid, register it
             symbolTable.registerVariable($2, $5, $8);
         }
```

**Error Output:**
```
SEMANTIC ERROR at line 2: Invalid domain for 'Col1': 
min value (5) cannot be greater than max value (2)
```

---

### 3C. UNDEFINED VARIABLE IN CONSTRAINT

**Location:** `src/parser.y` (constraint_def rule, lines ~155-180)

**What it checks:**
Both variables in constraint must be declared before use

**Example:**
```solva
constraint Col1 != Col2;  ← ERROR: Col2 never declared!
```

**Error Code (parser.y constraint_def rule):**
```cpp
constraint_def : KEYWORD_CONSTRAINT ID OPERATOR ID SEMICOLON
               {
                   // ❌ ERROR CHECK: Left variable exists?
                   if (!symbolTable.variableExists($2)) {
                       std::cerr << "SEMANTIC ERROR at line " << yylineno 
                                 << ": Variable '" << $2 
                                 << "' used in constraint but never declared!" 
                                 << std::endl;
                       YYERROR;
                   }
                   
                   // ❌ ERROR CHECK: Right variable exists?
                   if (!symbolTable.variableExists($4)) {
                       std::cerr << "SEMANTIC ERROR at line " << yylineno 
                                 << ": Variable '" << $4 
                                 << "' used in constraint but never declared!" 
                                 << std::endl;
                       YYERROR;
                   }
                   
                   // ✓ Both exist, add rule
                   logicEngine.addRule($2, $3, $4);
               }
```

**Error Output:**
```
SEMANTIC ERROR at line 5: Variable 'Col2' used in constraint 
but never declared!
```

---

### 3D. UNDEFINED VARIABLE IN ASSIGNMENT

**Location:** `src/parser.y` (assignment_stmt rule, lines ~189-230)

**What it checks:**
Variable must be declared before assigning value to it

**Example:**
```solva
Col1 = 2;  ← ERROR: Col1 never declared!
```

**Error Code (parser.y assignment_stmt rule):**
```cpp
assignment_stmt : ID ASSIGN INT SEMICOLON
                {
                    // ❌ ERROR CHECK: Variable declared?
                    if (!symbolTable.variableExists($1)) {
                        std::cerr << "SEMANTIC ERROR at line " << yylineno 
                                  << ": Variable '" << $1 
                                  << "' used in assignment but never declared!" 
                                  << std::endl;
                        YYERROR;
                    }
                    
                    // ... more checks ...
                }
```

**Error Output:**
```
SEMANTIC ERROR at line 10: Variable 'Col1' used in assignment 
but never declared!
```

---

### 3E. VALUE OUT OF DOMAIN ERROR

**Location:** `src/parser.y` (assignment_stmt rule, lines ~211-218) + `src/symbol_table.cpp`

**What it checks:**
Assigned value must be within [min..max] domain

**Example:**
```solva
var Col1 in [1..4];
Col1 = 10;  ← ERROR: 10 not in [1..4]!
```

**Error Code (parser.y assignment_stmt rule):**
```cpp
assignment_stmt : ID ASSIGN INT SEMICOLON
                {
                    // ... variable exists check ...
                    
                    // ❌ ERROR CHECK: Value in domain?
                    if (!symbolTable.isValueInDomain($1, $3)) {
                        VariableDomain domain = symbolTable.getDomain($1);
                        std::cerr << "VALUE ERROR at line " << yylineno 
                                  << ": Cannot assign " << $1 << " = " << $3 
                                  << " (value out of domain [" 
                                  << domain.minValue << ".." 
                                  << domain.maxValue << "])" << std::endl;
                        YYERROR;
                    }
                    
                    // ... more checks ...
                }
```

**Checked by (symbol_table.cpp):**
```cpp
bool SymbolTable::isValueInDomain(const std::string& name, int val) const {
    auto it = domains.find(name);
    if (it == domains.end()) return false;
    
    // ✓ Check: minValue ≤ val ≤ maxValue
    return (val >= it->second.minValue) && (val <= it->second.maxValue);
}
```

**Error Output:**
```
VALUE ERROR at line 11: Cannot assign Col1 = 10 
(value out of domain [1..4])
```

---

### 3F. DUPLICATE ASSIGNMENT ERROR

**Location:** `src/parser.y` (assignment_stmt rule, lines ~219-226) + `src/symbol_table.cpp`

**What it checks:**
Variable cannot be assigned twice in the same section

**Example:**
```solva
Manual Inputs:
Col1 = 1;
Col1 = 2;  ← ERROR: Col1 assigned twice!
```

**Error Code (parser.y assignment_stmt rule):**
```cpp
assignment_stmt : ID ASSIGN INT SEMICOLON
                {
                    // ... previous checks ...
                    
                    // ❌ ERROR CHECK: Already assigned?
                    if (symbolTable.hasValue($1)) {
                        std::cerr << "SEMANTIC ERROR at line " << yylineno 
                                  << ": Variable '" << $1 
                                  << "' has already been assigned a value!" 
                                  << std::endl;
                        YYERROR;
                    }
                    
                    // ... more checks ...
                }
```

**Checked by (symbol_table.cpp):**
```cpp
bool SymbolTable::hasValue(const std::string& name) const {
    return memory.find(name) != memory.end();
}
```

**Error Output:**
```
SEMANTIC ERROR at line 13: Variable 'Col1' has already been assigned a value!
```

---

### 3G. CONSTRAINT VIOLATION ERROR

**Location:** `src/parser.y` (assignment_stmt rule, lines ~227-234) + `src/logic_engine.cpp`

**What it checks:**
Assigned value must not violate any constraint rule

**Example 1 - Inequality Constraint:**
```solva
var Col1 in [1..4];
var Col2 in [1..4];
constraint Col1 != Col2;

Manual Inputs:
Col1 = 2;
Col2 = 2;  ← ERROR: Violates Col1 != Col2!
```

**Example 2 - Comparison Constraint:**
```solva
var Alice in [1..4];
var Bob in [1..4];
constraint Alice > Bob;

Manual Inputs:
Alice = 2;
Bob = 5;   ← ERROR: 2 is NOT > 5!
```

**Example 3 - Absolute Difference Constraint:**
```solva
var Col1 in [1..4];
var Col2 in [1..4];
constraint abs(Col1 - Col2) != 1;

Manual Inputs:
Col1 = 1;
Col2 = 2;  ← ERROR: |1-2| = 1, violates != 1!
```

**Error Code (parser.y assignment_stmt rule):**
```cpp
assignment_stmt : ID ASSIGN INT SEMICOLON
                {
                    // ... all previous checks ...
                    
                    // ❌ ERROR CHECK: Violates constraints?
                    if (!logicEngine.isMoveValid($1, $3, symbolTable)) {
                        std::cerr << "CONSTRAINT VIOLATION at line " << yylineno 
                                  << ": Cannot assign " << $1 << " = " << $3 
                                  << " (violates existing constraints)" << std::endl;
                        YYERROR;
                    }
                    
                    // ✓ Valid, assign it
                    symbolTable.setValue($1, $3);
                }
```

**Handled by (logic_engine.cpp - isMoveValid function):**

```cpp
bool LogicEngine::isMoveValid(const std::string& targetVar, 
                              int targetValue, 
                              const SymbolTable& st) const {
    // Check EACH constraint rule
    for (const auto& rule : rules) {
        // Is target variable involved in this rule?
        bool involved = (rule.var1 == targetVar || rule.var2 == targetVar);
        if (!involved) continue;
        
        // Get the OTHER variable's value (if already assigned)
        int otherValue = ...;
        if (!st.hasValue(otherVar)) continue;  // Skip if other not assigned yet
        
        // ❌ CHECK: Does this assignment satisfy the rule?
        bool satisfied = false;
        
        if (rule.isAbs) {
            // ABSOLUTE DIFFERENCE CONSTRAINT
            int absDiff = std::abs(targetValue - otherValue);
            
            if (rule.op == "!=") {
                satisfied = (absDiff != rule.intVal);
            } else if (rule.op == "==") {
                satisfied = (absDiff == rule.intVal);
            } else if (rule.op == ">") {
                satisfied = (absDiff > rule.intVal);
            }
            // ... other operators ...
        } else {
            // BINARY CONSTRAINT (var OP var)
            
            if (rule.op == "!=") {
                satisfied = (targetValue != otherValue);
            } else if (rule.op == "==") {
                satisfied = (targetValue == otherValue);
            } else if (rule.op == ">") {
                // Must check position: is targetVar var1 or var2?
                if (rule.var1 == targetVar) {
                    satisfied = (targetValue > otherValue);
                } else {
                    satisfied = (otherValue > targetValue);
                }
            }
            // ... other operators ...
        }
        
        // If ANY constraint is violated, return false
        if (!satisfied) {
            return false;
        }
    }
    
    return true;  // All constraints satisfied
}
```

**Error Output:**
```
CONSTRAINT VIOLATION at line 15: Cannot assign Col2 = 2 
(violates existing constraints)
```

---

## 4. ERROR HANDLING SUMMARY TABLE

| Error Type | Location | Phase | How Detected | Example |
|-----------|----------|-------|--------------|---------|
| **Lexical Errors** | | | | |
| Unrecognized character | lexer.l line 47-52 | LEXICAL | Regex pattern mismatch | `@` `#` `"string"` |
| | | | | |
| **Syntax Errors** | | | | |
| Missing semicolon | parser.y var_decl | SYNTACTIC | Expected `;` not found | `var X in [1..5]` |
| Missing keyword | parser.y program | SYNTACTIC | Expected keyword not found | `X in [1..5];` |
| Wrong brackets | parser.y var_decl | SYNTACTIC | Expected `[` or `]` | `var X in (1..5);` |
| Missing colon | parser.y input_section | SYNTACTIC | Expected `:` after section | `Manual Inputs` (no `:`) |
| | | | | |
| **Semantic Errors** | | | | |
| Duplicate variable | parser.y var_decl | SEMANTIC | symbolTable.variableExists() | Two `var Col1` |
| Invalid domain (min>max) | parser.y var_decl | SEMANTIC | $5 > $8 check | `var X in [5..2];` |
| Undefined var in constraint | parser.y constraint_def | SEMANTIC | symbolTable.variableExists() | `constraint X != Y;` (Y undefined) |
| Undefined var in assignment | parser.y assignment_stmt | SEMANTIC | symbolTable.variableExists() | `X = 5;` (X undefined) |
| Value out of domain | parser.y assignment_stmt | SEMANTIC | symbolTable.isValueInDomain() | `var X in [1..4]; X = 10;` |
| Duplicate assignment | parser.y assignment_stmt | SEMANTIC | symbolTable.hasValue() | `X = 1; X = 2;` |
| Constraint violation | parser.y assignment_stmt | SEMANTIC | logicEngine.isMoveValid() | `constraint X != Y; X=2; Y=2;` |

---

## 5. COMPILATION FLOW WITH ERROR DETECTION

```
Input File (sudoku.solva)
    ↓
┌──────────────────────────────────┐
│ LEXER (lexer.l)                  │
│ Function: yylex()                │
│ Returns: TOKENS (or error)       │
│ Errors: LEXICAL ERRORS           │
│ Example: Invalid character '@'   │
└──────────────────────────────────┘
    ↓ (if lexical errors) → STOP & REPORT ERROR
    ↓ (if OK)
Token Stream
    ↓
┌──────────────────────────────────┐
│ PARSER (parser.y - Bison)        │
│ Function: yyparse()              │
│ Matches: Token sequence rules    │
│ Errors: SYNTAX ERRORS            │
│ Example: Missing semicolon       │
└──────────────────────────────────┘
    ↓ (if syntax errors) → STOP & REPORT ERROR
    ↓ (if OK)
AST Built
    ↓
┌──────────────────────────────────┐
│ SEMANTIC CHECKS (parser.y)       │
│ Location: var_decl rule          │
│ Errors: SEMANTIC ERRORS          │
│ ✓ Duplicate variable check       │
│ ✓ Invalid domain check           │
└──────────────────────────────────┘
    ↓ (if semantic errors) → STOP & REPORT ERROR
    ↓ (if OK)
Symbol Table Populated
    ↓
┌──────────────────────────────────┐
│ REFERENCE & CONSTRAINT CHECKS    │
│ Location: constraint_def, assignment_stmt │
│ Errors: SEMANTIC ERRORS          │
│ ✓ Undefined variable check       │
│ ✓ Domain membership check        │
│ ✓ Duplicate assignment check     │
│ ✓ Constraint violation check     │
└──────────────────────────────────┘
    ↓ (if semantic errors) → STOP & REPORT ERROR
    ↓ (if OK)
Valid Program with All Constraints Satisfied
    ↓
┌──────────────────────────────────┐
│ LOGIC ENGINE (logic_engine.cpp)  │
│ Solves for remaining variables   │
│ Finds valid solution (or UNSAT)  │
└──────────────────────────────────┘
    ↓
Output Results & Visualization
```

---

## 6. KEY FILES & FUNCTIONS FOR ERROR HANDLING

| Component | File | Key Functions/Lines | Error Checks |
|-----------|------|-------------------|--------------|
| Lexer | lexer.l | Line 47-52 (.) rule | Invalid characters |
| Parser Grammar | parser.y | Lines 88-103 (var_decl) | Syntax validation |
| Duplicate Check | parser.y + symbol_table.cpp | var_decl rule + variableExists() | Duplicate vars |
| Domain Check | parser.y + symbol_table.cpp | var_decl rule + isValueInDomain() | min > max, value in range |
| Reference Check | parser.y + symbol_table.cpp | constraint_def, assignment_stmt + variableExists() | Undefined vars |
| Constraint Check | parser.y + logic_engine.cpp | assignment_stmt + isMoveValid() | Rule violations |
| Error Reporting | parser.y | yyerror() function | All errors printed |
| Line Tracking | lexer.l | %option yylineno | Line numbers in errors |
