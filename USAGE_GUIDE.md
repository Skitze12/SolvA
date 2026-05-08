# SolvA: Syntax-Directed CSP Validator - Usage Guide

## Overview
SolvA is a Domain-Specific Language (DSL) compiler that validates Constraint Satisfaction Problems (CSPs) through syntax-directed translation. It combines compilation techniques with constraint validation, treating logic violations as syntax-level errors.

## Prerequisites

### Required Tools
- **gcc/g++** - C/C++ compiler
- **flex** - Lexical analyzer generator
- **bison** - Parser generator
- **make** - Build automation
- **graphviz** (optional) - For visualizing the AST

### Installation on WSL/Linux
```bash
sudo apt update
sudo apt install build-essential flex bison graphviz
```

## Building the Project

### Step 1: Navigate to the Project Directory
```bash
cd /mnt/d/CC\ Project/SolvA
```

### Step 2: Clean Previous Builds
```bash
make clean
```

### Step 3: Build the Entire Project
```bash
make all
```

This will:
1. Run Bison to generate the parser
2. Run Flex to generate the lexer
3. Compile all C++ source files
4. Link everything into the `solva` executable

### Build Output
After successful compilation, you should see:
```
bison -d -o src/parser.cpp src/parser.y
g++ -std=c++11 -Wall -Wextra -I./include -c -o src/main.o src/main.cpp
g++ -std=c++11 -Wall -Wextra -I./include -c -o src/ast.o src/ast.cpp
g++ -std=c++11 -Wall -Wextra -I./include -c -o src/logic_engine.o src/logic_engine.cpp
g++ -std=c++11 -Wall -Wextra -I./include -c -o src/symbol_table.o src/symbol_table.cpp
flex -o src/lex.yy.c src/lexer.l
g++ -std=c++11 -Wall -Wextra -I./include -c -o src/lex.yy.o src/lex.yy.c
g++ -std=c++11 -Wall -Wextra -I./include -c -o src/parser.o src/parser.cpp
g++ -std=c++11 -Wall -Wextra -I./include -o solva src/main.o src/ast.o src/logic_engine.o src/symbol_table.o src/lex.yy.o src/parser.o
```

## SolvA Syntax

### Variable Declaration
```solva
var <name> in [<min>..<max>];
```

Example:
```solva
var Col1 in [1..4];
var Sindh in [1..9];
```

### Constraints
```solva
constraint <var1> <operator> <var2>;
```

Supported operators: `==`, `!=`, `<`, `>`, `<=`, `>=`

Example:
```solva
constraint Col1 != Col2;
constraint A > B;
```

### Assignments (Solutions)
```solva
<var> = <value>;
```

Example:
```solva
Col1 = 2;
Sindh = 5;
```

## Running Examples

### Example 1: N-Queens Problem
```bash
./solva examples/nqueens.solva
```

This validates a 4-queens solution where each queen must be in a different column.

**Expected Output:**
```
--- Starting SolvA Validation ---
Input file: examples/nqueens.solva

✓ SUCCESS: The provided CSP solution is syntactically and logically valid!
Parse tree saved to output.dot
AST visualization generated successfully.
```

### Example 2: Sudoku (Simplified)
```bash
./solva examples/sudoku.solva
```

This validates a simplified sudoku row where all cells must have different values.

### Example 3: Simple Test
```bash
./solva testing/testing.solva
```

### Example 4: Invalid Solution (Testing Error Detection)
```bash
# Create a test file with conflicting assignments
cat > testing/invalid_test.solva << 'EOF'
var A in [1..10];
var B in [1..10];

constraint A != B;

A = 5;
B = 5;
EOF

# Run it - should fail
./solva testing/invalid_test.solva
```

**Expected Output:**
```
--- Starting SolvA Validation ---
Input file: testing/invalid_test.solva
CONSTRAINT VIOLATION: A != B (attempted to set B = 5, but A = 5)

✗ FAILED: Validation aborted due to logic/syntax errors.
```

## Understanding the Output

### On Success
1. **Console Output**: Success message with validation confirmation
2. **output.dot**: Generated Abstract Syntax Tree (AST) in Graphviz DOT format

### On Failure
- **Constraint Violation**: Shows which constraint was violated and why
- **Syntax Error**: Shows line number and parsing error
- **Lexical Error**: Shows unrecognized character in input

## Visualizing the AST

After a successful validation, `output.dot` is generated. Convert it to an image:

### PNG Format
```bash
dot -Tpng output.dot -o tree.png
```

### PDF Format
```bash
dot -Tpdf output.dot -o tree.pdf
```

### SVG Format (for web viewing)
```bash
dot -Tsvg output.dot -o tree.svg
```

Then view the generated image file.

## Creating Your Own Test Files

### Template
Create a `.solva` file with the following structure:

```solva
// Variable declarations
var Variable1 in [1..10];
var Variable2 in [1..10];

// Constraints
constraint Variable1 != Variable2;
constraint Variable1 < Variable2;

// Solution assignments
Variable1 = 3;
Variable2 = 7;
```

### Execution
```bash
./solva path/to/your/file.solva
```

## Troubleshooting

### Build Errors

**Error: `iostream: No such file or directory`**
- Ensure you're using g++ (not gcc) to compile
- Fixed in current Makefile

**Error: `'Node' does not name a type`**
- Parser header generation issue
- Run `make clean` then `make all`

### Runtime Errors

**Error: `Could not open file`**
```bash
# Make sure the file exists and path is correct
ls -la examples/
./solva examples/nqueens.solva  # Use correct path
```

**Error: `CONSTRAINT VIOLATION`**
- Your assignments don't satisfy the constraints
- Check variable values against constraint definitions
- Verify constraint logic is correct

## Project Structure

```
SolvA/
├── src/
│   ├── main.cpp          # Entry point
│   ├── lexer.l           # Flex lexical analyzer
│   ├── parser.y          # Bison parser
│   ├── ast.cpp           # AST node implementations
│   ├── logic_engine.cpp  # Constraint validator
│   └── symbol_table.cpp  # State management
├── include/
│   ├── ast.h             # AST node definitions
│   ├── logic_engine.h    # Logic engine header
│   └── symbol_table.h    # Symbol table header
├── examples/
│   ├── nqueens.solva     # N-Queens example
│   └── sudoku.solva      # Sudoku example
├── testing/
│   └── testing.solva     # Simple test
├── Makefile              # Build configuration
└── README.md             # This file
```

## How It Works

1. **Lexical Analysis (Flex)**: Converts `.solva` text into tokens
2. **Syntax Analysis (Bison)**: Parses tokens according to grammar rules
3. **Logic Engine**: Validates each assignment against constraints in real-time
4. **Symbol Table**: Tracks variable values during parsing
5. **AST Generation**: Builds tree representation of the program
6. **Output**: Generates DOT format for visualization

## Make Commands

```bash
make all              # Build the entire project
make clean            # Remove all build artifacts
make run              # Build and run with nqueens.solva
make test-nqueens     # Test N-Queens example
make test-sudoku      # Test Sudoku example
make test-simple      # Test simple test file
```

## Example Test Cases

### Valid Map Coloring
```solva
var Sindh in [1..3];
var Punjab in [1..3];
var Balochistan in [1..3];

constraint Sindh != Punjab;
constraint Sindh != Balochistan;
constraint Punjab != Balochistan;

Sindh = 1;
Punjab = 2;
Balochistan = 3;
```

### Invalid Graph Coloring
```solva
var A in [1..2];
var B in [1..2];

constraint A != B;

A = 1;
B = 1;  // VIOLATES: A != B
```

## Next Steps

1. Build the project: `make all`
2. Run an example: `./solva examples/nqueens.solva`
3. Create your own `.solva` file
4. Validate your CSP solutions
5. Visualize the AST with Graphviz

---

**SolvA** demonstrates how compiler design principles can be applied to constraint validation problems. By embedding logic into syntax analysis, it ensures correctness and provides immediate feedback during parsing.
