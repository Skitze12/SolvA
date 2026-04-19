# SolvA
📘 SolvA: Syntax-Directed CSP Validator — Abstract Summary
🔍 Overview

SolvA is a Domain-Specific Language (DSL) designed to model and validate Constraint Satisfaction Problems (CSPs).
Unlike traditional compilers, SolvA integrates logical validation directly into the parsing phase, treating constraint violations as syntax-level errors.

🧠 Core Concept
Combines compilation techniques with constraint validation
Uses Syntax-Directed Translation (SDT) to enforce logic during parsing
Eliminates the need for a backend execution engine
Ensures invalid states are rejected immediately
🏗️ High-Level Architecture
📥 Input
.solva file containing:
Variables
Domains
Constraints
Assignment sequence (moves)
⚙️ Processing Pipeline
1. Lexical Analysis (Flex)
Converts raw text into tokens
Uses Regular Expressions
Identifies:
Keywords (var, constraint)
Identifiers (A, B)
Operators (=, !=)
Symbols (;)

Output: Token stream

2. Syntax Analysis (Bison)
Validates structure using Context-Free Grammar (CFG)
Matches token sequences to grammar rules
Executes semantic actions on rule matches

Example Rule:

assignment : ID '=' INT ';'

Output:

Triggers validation logic
Builds Abstract Syntax Tree (AST)
3. Logic Engine (CSP Validator)
Acts as the decision-making component
Validates assignments against constraints
Checks consistency with:
Existing assignments
Defined rules

Behavior:

Valid move → Update state
Invalid move → Raise error and halt
4. State Table (Symbol Table)
Stores:
Variable-value mappings
Maintains current system state
Used for conflict detection
5. AST Visualizer
Converts AST into DOT format
Enables graphical visualization using Graphviz
🔄 Data Flow Summary
Input (.solva)
     ↓
Lexer (Flex) → Tokens
     ↓
Parser (Bison) → Grammar Validation + Actions
     ↓
Logic Engine → Constraint Checking
     ↓
State Table Update
     ↓
Output (Success / Error + AST)
📤 Output Behavior
✅ On Success
Validation confirmation
AST generated (output.dot)
Optional visualization (tree.png)
❌ On Failure
Immediate termination
Error type:
Syntax Error (grammar violation)
Logic-Syntax Error (constraint violation)
⚡ Key Features
Syntax-Directed Validation
Logic enforced during parsing
Immediate Error Detection
Prevents invalid states early
Modular Design
Clear separation of components
Visualization Support
AST generation via Graphviz
Efficient Execution
No runtime solving required
🧩 Component Breakdown
📄 lexer.l (Flex)
Tokenizes input
Uses regex-based pattern matching
📄 parser.y (Bison)
Enforces grammar rules
Executes validation logic
Builds AST
📄 logic_engine.cpp
Validates CSP constraints
Core reasoning unit
📄 symbol_table.h
Stores variable states
Ensures consistency
📄 visualizer.cpp
Converts AST to DOT format
Enables graphical representation
🔗 Lexer vs Parser
Feature	Lexer (Flex)	Parser (Bison)
Role	Tokenization	Grammar validation
Input	Raw text	Tokens
Output	Tokens	AST + Logic execution
Focus	Pattern matching	Structure + semantics
Error Type	Invalid characters	Syntax / logic errors
Analogy	Reads words	Validates sentences
🧠 How It Works (Execution Flow)
Phase 1: Constraint Definition
Constraints parsed and stored
Phase 2: Assignment Processing
Each assignment triggers validation
Checked against:
Existing assignments
Constraints
Phase 3: Validation
If valid → update state
If invalid → terminate with error
🎯 Design Philosophy
Treat logic violations as syntax errors
Shift validation left (early in compilation)
Simplify CSP solving into validation problem
Ensure correctness before completion
🚀 Conclusion

SolvA demonstrates how compiler design principles can be extended beyond traditional programming languages to solve constraint validation problems efficiently. By embedding logic into syntax analysis, it ensures correctness, reduces complexity, and provides immediate feedback—making it a powerful tool for CSP modeling.
