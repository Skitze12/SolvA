#include "../include/logic_engine.h"
#include <iostream>
#include <cmath>

void LogicEngine::addRule(const std::string& v1, const std::string& op, const std::string& v2) {
    rules.push_back({false, v1, op, v2, 0});
}

void LogicEngine::addRule(const std::string& v1, const std::string& v2, const std::string& op, int intVal) {
    rules.push_back({true, v1, op, v2, intVal});
}

bool LogicEngine::isMoveValid(const std::string& targetVar, int targetValue, const SymbolTable& st) const {
    // Check each constraint to see if this assignment violates any of them
    for (const auto& rule : rules) {
        bool targetVarInvolvedAsVar1 = (rule.var1 == targetVar);
        bool targetVarInvolvedAsVar2 = (rule.var2 == targetVar);
        
        if (!targetVarInvolvedAsVar1 && !targetVarInvolvedAsVar2) {
            // This constraint doesn't involve our target variable, so it's not relevant yet
            continue;
        }
        
        // Get the value of the other variable in the constraint (if assigned)
        int otherValue = -1;
        std::string otherVar;
        
        if (targetVarInvolvedAsVar1) {
            // The constraint is: targetVar op var2
            // We need var2's current value
            otherVar = rule.var2;
            if (!st.hasValue(otherVar)) {
                // The other variable hasn't been assigned yet, so we can't check this constraint yet
                continue;
            }
            otherValue = st.getValue(otherVar);
        } else {
            // The constraint is: var1 op targetVar
            // We need var1's current value
            otherVar = rule.var1;
            if (!st.hasValue(otherVar)) {
                // The other variable hasn't been assigned yet, so we can't check this constraint yet
                continue;
            }
            otherValue = st.getValue(otherVar);
        }
        
        // Now check if targetValue and otherValue satisfy the constraint
        bool constraintSatisfied = false;
        
        if (rule.isAbs) {
            int absDiff = std::abs(targetValue - otherValue);
            if (rule.op == "==") {
                constraintSatisfied = (absDiff == rule.intVal);
            } else if (rule.op == "!=") {
                constraintSatisfied = (absDiff != rule.intVal);
            } else if (rule.op == "<") {
                constraintSatisfied = (absDiff < rule.intVal);
            } else if (rule.op == ">") {
                constraintSatisfied = (absDiff > rule.intVal);
            } else if (rule.op == "<=") {
                constraintSatisfied = (absDiff <= rule.intVal);
            } else if (rule.op == ">=") {
                constraintSatisfied = (absDiff >= rule.intVal);
            }
        } else {
            if (rule.op == "==") {
                constraintSatisfied = (targetValue == otherValue);
            } else if (rule.op == "!=") {
                constraintSatisfied = (targetValue != otherValue);
            } else if (rule.op == "<") {
                if (targetVarInvolvedAsVar1) {
                    constraintSatisfied = (targetValue < otherValue);
                } else {
                    constraintSatisfied = (otherValue < targetValue);
                }
            } else if (rule.op == ">") {
                if (targetVarInvolvedAsVar1) {
                    constraintSatisfied = (targetValue > otherValue);
                } else {
                    constraintSatisfied = (otherValue > targetValue);
                }
            } else if (rule.op == "<=") {
                if (targetVarInvolvedAsVar1) {
                    constraintSatisfied = (targetValue <= otherValue);
                } else {
                    constraintSatisfied = (otherValue <= targetValue);
                }
            } else if (rule.op == ">=") {
                if (targetVarInvolvedAsVar1) {
                    constraintSatisfied = (targetValue >= otherValue);
                } else {
                    constraintSatisfied = (otherValue >= targetValue);
                }
            }
        }
        
        if (!constraintSatisfied) {
            // This constraint is violated
            return false;
        }
    }
    
    return true; // All constraints are satisfied
}

bool LogicEngine::solve(size_t varIndex, const std::vector<std::string>& variables, SymbolTable& st) const {
    // Base case: all variables assigned
    if (varIndex == variables.size()) {
        return true;
    }
    
    const std::string& currentVar = variables[varIndex];
    VariableDomain domain = st.getDomain(currentVar);
    
    // Try each value in the variable's domain
    for (int value = domain.minValue; value <= domain.maxValue; ++value) {
        if (isMoveValid(currentVar, value, st)) {
            // This value is valid, so assign it
            st.setValue(currentVar, value);
            
            // Recursively try to solve the rest
            if (solve(varIndex + 1, variables, st)) {
                return true;
            }
            
            // Backtrack: remove the assignment if it didn't lead to a solution
            st.unsetValue(currentVar);
        }
    }
    
    return false;
}

bool LogicEngine::findSolution(SymbolTable& st) const {
    // Get all registered variables
    std::vector<std::string> variables = st.getAllVariables();
    
    // Clear any existing assignments
    st.clearAssignments();
    
    // Use backtracking to find a valid solution
    return solve(0, variables, st);
}
