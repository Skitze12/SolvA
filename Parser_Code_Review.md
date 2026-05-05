# 🔍 SolvA: Parser Implementation Review

This document breaks down the completed implementation of `src/parser.y`. It serves as a guide for reviewing the grammar, understanding the syntax-directed logic triggers, and identifying the exact dependencies required from the rest of the team to finalize the compiler.

## 🚀 Overview

The parser is responsible for:
1. Validating the structural Context-Free Grammar (CFG) of `.solva` files.
2. Building the Abstract Syntax Tree (AST) dynamically.
3. Triggering real-time constraint checks using the `LogicEngine`.

---

## 🏗️ 1. Global State and AST Construction

The parser maintains a global state to build the AST and track variables.

```cpp
SymbolTable symbolTable;
LogicEngine logicEngine;
ProgramNode* astRoot = nullptr; 
```
- **Dynamic Construction:** The `astRoot` is built incrementally. As each statement is parsed, it is immediately attached as a child via `astRoot->addChild($1)`.
- **Global Safety:** If a logic error causes parsing to crash, the global `astRoot` is safely deleted inside the `yyerror()` function.

---

## 🚨 2. Memory Safety (`%destructor`)

We utilize Bison's `%destructor` directive to prevent memory leaks during logic failures.
```bison
%destructor { delete $$; } <str_val>
%destructor { delete $$; } <node_val>
```
If the parser calls `YYABORT` (e.g., when a CSP constraint fails), Bison automatically sweeps the parser stack and deletes any unattached AST nodes or string tokens.

---

## ⚙️ 3. Syntax-Directed Logic Hooks

The core of SolvA's innovation is the real-time logic validation embedded directly within the Bison action blocks.

### Constraints
When a constraint like `constraint Sindh != Punjab;` is parsed:
```bison
constraint_def : KEYWORD_CONSTRAINT ID OPERATOR ID SEMICOLON
               {
                   logicEngine.addRule(*$2, *$3, *$4);
                   $$ = new ConstraintNode(*$2, *$3, *$4);
                   delete $2; delete $3; delete $4;
               }
```
**Behavior:** It registers the rule with the `LogicEngine` and builds the node.

### Assignments (The Core Trigger)
When an assignment like `Sindh = 1;` is parsed:
```bison
assignment : ID ASSIGN INT SEMICOLON
           {
               if (!logicEngine.isMoveValid(*$1, $3, symbolTable)) {
                   yyerror("Logic conflict detected! Invalid assignment.");
                   delete $1; 
                   YYABORT; // Halt immediately!
               }
               symbolTable.setValue(*$1, $3);
               $$ = new AssignmentNode(*$1, $3);
               delete $1;
           }
```
**Behavior:** It asks the `LogicEngine` if the move is valid based on previous constraints. If it fails, it throws a syntax error and aborts parsing instantly. If it succeeds, it updates the `SymbolTable`.

---

## 📝 4. Action Items for the Rest of the Team (TODOs)

There are a few temporary placeholders left in `parser.y` so that the file will successfully compile today. Once the rest of the team finishes their components, these lines need to be uncommented.

### ➡️ For the Logic Engine Team:
- **Task:** The parser needs to register variable domains (e.g., `var Sindh in [1..3]`).
- **Required:** Implement `void addDomain(const std::string& name, int min, int max)` in `logic_engine.h`.
- **In `parser.y`:** Go to the `var_decl` rule and uncomment the line: 
  `// logicEngine.addDomain(*$2, $5, $8);`

### ➡️ For the AST / Visualizer Team:
- **Task:** Variable declarations need to appear in the AST visualization for Graphviz.
- **Required:** Implement a `VarDeclNode` class in `ast.h` that inherits from `Node`.
- **In `parser.y`:** Go to the `var_decl` rule, remove `$$ = nullptr;`, and uncomment: 
  `// $$ = new VarDeclNode(*$2, $5, $8);`
