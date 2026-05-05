### **CS4031 – Compiler Construction**

### **Course Project Proposal**

**Spring 2026**

| Project Title: | SolvA: The Syntax-Directed Logic Validator |
| :---- | :---- |
| **Theme:** | **Logic-Embedded Compiler Design / Declarative DSL for CSP** |

**Team Members:**

1. **Shaheer Uddin Ahmed, 23K-0649**  
2. **M. Faizan Jawaid, 23K-0688**  
3. **Yahya Sheikh, 23K-0718**  
4. **Aliyan Navaid, 23K-0688**  
   ---

   ### **1\. Language Concept**

**SolvA** is a Domain-Specific Language (DSL) designed to model and validate **Constraint Satisfaction Problems (CSP)** through real-time syntax analysis. Unlike traditional compilers that merely translate text into instructions, SolvA treats logical consistency as a grammatical requirement. By utilizing **Syntax-Directed Translation (SDT)**, the compiler acts as the solver: it builds a state-aware Parse Tree that rejects variable assignments if they violate previously defined logical constraints. It is intended for AI researchers and students to quickly model, test, and visualize the search space of discrete problems like Sudoku, Map Coloring, or N-Queens.

---

### **2\. Key Features**

* **Generic CSP Modeling:** Support for defining abstract variables (e.g., A, B, C), finite domains (e.g., \[1..9\]), and logical constraints (e.g., \!=, \==, \>) in a declarative format.  
* **Syntax-Directed Logic Validation:** Embedded C++ actions within the Bison grammar that validate "moves" in real-time. If a user-provided solution breaks a rule, the compiler triggers a specialized **Logic-Syntax Error**.  
* **Dynamic State Table:** A robust symbol table implementation that tracks the current "state" of the world as assignments are parsed, allowing the compiler to perform look-ahead or conflict checks.  
* **Automated Parse Tree Visualization:** Integration with **Graphviz (DOT)** to generate a visual representation of the decision tree, showing exactly which branch led to a valid solution or a logical failure.  
* **Attribute Grammar Support:** Use of inherited and synthesized attributes to pass constraint requirements down through the parse tree for efficient validation.  
  ---

  ### **3\. Example Program (Map Coloring Problem)**

  Plaintext  
* // SolvA code to validate a 3-color Map Problem  
* // Rule: Neighbors cannot have the same color  
*   
* var Sindh, Punjab, Balochistan in \[Red, Green, Blue\]  
*   
* // Define Constraints  
* constraint Sindh \!= Punjab  
* constraint Sindh \!= Balochistan  
* constraint Punjab \!= Balochistan  
*   
* // Provide a solution for validation  
* Sindh \= Red;        // Accepted: Parser updates state  
* Punjab \= Green;     // Accepted: Parser updates state  
* Balochistan \= Red;  // Error: Syntax Error at Line 11: Logic conflict with Sindh (Rule: \!=)  
    
  ---

  ### **4\. Target Output**

* **Direct Validation Results:** Terminal output indicating "Compilation Successful" (Valid Solution) or specific line-number Logic Errors.  
* **Visual Parse Tree:** A .dot file representing the assignment hierarchy and the state-check nodes.  
  ---

  ### **5\. Expected Challenges**

* **Challenge 1: Generic Constraint Mapping:** Designing a grammar flexible enough to handle various logical operators (like abs() or inequalities) without creating an ambiguous or overly complex CFG.  
* **Challenge 2: Real-time Conflict Resolution:** Correctly implementing the "Action" logic in Bison to efficiently check the existing state table every time a new assignment node is added to the Parse Tree.  
  ---

