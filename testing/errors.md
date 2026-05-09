# SolvA Error Handling Test Cases

## Error Categories and Examples

### 1. LEXICAL ERRORS
- Unrecognized characters: `@`, `#`, `$`, etc.
- Unsupported strings: `"string"` or `'string'`

**Example:**
```solva
var A in [1..3]  @ error
```

**Error:** LEXICAL ERROR at line 1: Unrecognized character '@'

---

### 2. SYNTAX ERRORS
- Missing semicolons
- Missing colons after section headers
- Invalid domain specification
- Missing brackets in domain

**Examples:**
```solva
var A in [1..3]      // Missing semicolon
constraint A != B    // Missing semicolon
Manual Inputs        // Missing colon
```

**Error:** SYNTAX ERROR at line X

---

### 3. SEMANTIC ERRORS - VARIABLE DECLARATION

**A) Duplicate Variable Declaration**
```solva
var A in [1..5];
var A in [1..10];
```
**Error:** SEMANTIC ERROR at line 2: Variable 'A' has already been declared!

**B) Invalid Domain (min > max)**
```solva
var B in [10..5];
```
**Error:** SEMANTIC ERROR at line 1: Invalid domain for 'B': min value (10) cannot be greater than max value (5)

---

### 4. SEMANTIC ERRORS - CONSTRAINT

**A) Undefined Variable in Constraint**
```solva
var A in [1..5];
constraint A != B;
```
**Error:** SEMANTIC ERROR at line 2: Variable 'B' used in constraint but never declared!

**B) Multiple Undefined Variables**
```solva
var A in [1..5];
constraint X != Y;
```
**Error:** SEMANTIC ERROR at line 2: Variable 'X' used in constraint but never declared!

---

### 5. SEMANTIC ERRORS - ASSIGNMENT

**A) Undefined Variable in Assignment**
```solva
var A in [1..5];
B = 3;
```
**Error:** SEMANTIC ERROR at line 2: Variable 'B' used in assignment but never declared!

**B) Value Out of Domain**
```solva
var A in [1..5];
Manual Inputs:
A = 10;
```
**Error:** VALUE ERROR at line 3: Cannot assign A = 10 (value out of domain [1..5])

**C) Duplicate Assignment**
```solva
Manual Inputs:
A = 2;
A = 3;
```
**Error:** SEMANTIC ERROR at line 3: Variable 'A' has already been assigned a value!

---

### 6. CONSTRAINT VIOLATION

**A) Assignment Violates Constraint**
```solva
var A in [1..4];
var B in [1..4];
constraint A != B;

Manual Inputs:
A = 2;
B = 2;
```
**Error:** CONSTRAINT VIOLATION at line 7: Cannot assign B = 2 (violates existing constraints)

---

### 7. SEMANTIC ERRORS - MANUAL INPUTS

**A) Unassigned Variables in Manual Inputs Section**
```solva
var A in [1..5];
var B in [1..5];

Manual Inputs:
A = 2;
Machine Inputs:
```
**Error:** SEMANTIC ERROR: Manual Inputs section found but the following variables are not assigned:
  - B

---

### 8. FILE ERRORS

**A) File Not Found**
```bash
./solva nonexistent.solva
```
**Error:** FILE ERROR: Could not open file 'nonexistent.solva'
Possible causes:
  - File does not exist
  - Incorrect file path
  - Permission denied

---

## Error Handling Features

✅ Line number tracking for all errors
✅ Specific error type identification
✅ Contextual error messages
✅ Suggested fixes and common issues
✅ Semantic validation before AST generation
✅ Constraint violation detection
✅ Domain bound validation
✅ Variable existence checking
✅ Duplicate declaration detection

---

## Valid Example (No Errors)

```solva
// 4-Queens Problem
var Q1 in [1..4];
var Q2 in [1..4];
var Q3 in [1..4];
var Q4 in [1..4];

constraint Q1 != Q2;
constraint Q1 != Q3;
constraint Q1 != Q4;
constraint Q2 != Q3;
constraint Q2 != Q4;
constraint Q3 != Q4;

Manual Inputs:
Q1 = 1;
Q2 = 3;
Q3 = 4;
Q4 = 2;

Machine Inputs:
```

**Output:**
```
✓ SUCCESS: The provided CSP is syntactically, semantically, and logically valid!

--- Manual Assignments (from file) ---
  Q1 = 1 (MANUAL)
  Q2 = 3 (MANUAL)
  Q3 = 4 (MANUAL)
  Q4 = 2 (MANUAL)

--- Final Solution ---
All variables have been assigned manually. No machine-generated solutions needed.

Parse tree saved to output.dot
```
