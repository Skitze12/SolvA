#ifndef LOGIC_ENGINE_H
#define LOGIC_ENGINE_H

#include "symbol_table.h"
#include <vector>
#include <string>

// A structure to hold a rule like "A != B" or "abs(A - B) != 1"
struct Constraint {
    bool isAbs; // true if this is an absolute difference rule
    std::string var1;
    std::string op;
    std::string var2; // Used if isAbs == false
    int intVal;       // Used if isAbs == true
};

class LogicEngine {
private:
    // The master list of all constraints parsed from the code.
    std::vector<Constraint> rules;
    
    /*
     * WHAT IT DOES: Recursive backtracking solver for constraint satisfaction.
     * EXPECTS: Index of current variable, list of all variables, and symbol table.
     * RETURNS: True if a valid solution is found, false otherwise.
     */
    bool solve(size_t varIndex, const std::vector<std::string>& variables, SymbolTable& st) const;

public:
    LogicEngine() = default;
    ~LogicEngine() = default;

    /*
     * WHAT IT DOES: Saves a new rule into the engine's memory.
     * EXPECTS: The left variable, the operator (like "!="), and the right variable.
     * RETURNS: Nothing (void).
     */
    void addRule(const std::string& v1, const std::string& op, const std::string& v2);

    /*
     * WHAT IT DOES: Saves a new absolute difference rule into the engine's memory.
     * EXPECTS: The two variables to subtract, the operator, and the integer value.
     */
    void addRule(const std::string& v1, const std::string& v2, const std::string& op, int intVal);

    /*
     * WHAT IT DOES: The core validator. It checks if assigning a specific value 
     * to a specific variable breaks any stored rules.
     * EXPECTS: The variable being assigned, the number being assigned to it, 
     * and a read-only reference to the current Symbol Table.
     * RETURNS: True if the move is completely legal. False if it breaks a rule.
     */
    bool isMoveValid(const std::string& targetVar, int targetValue, const SymbolTable& st) const;
    
    /*
     * WHAT IT DOES: Automatically finds a valid solution that satisfies all constraints.
     * EXPECTS: A mutable reference to the symbol table with registered variables.
     * RETURNS: True if a valid solution is found, false if no solution exists.
     */
    bool findSolution(SymbolTable& st) const;
};

#endif