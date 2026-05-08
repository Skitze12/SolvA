#include "../include/logic_engine.h"
#include <iostream>

void LogicEngine::addRule(const std::string& v1, const std::string& op, const std::string& v2) {
    rules.push_back({v1, op, v2});
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
        
        if (!constraintSatisfied) {
            // This constraint is violated
            std::cerr << "CONSTRAINT VIOLATION: " << rule.var1 << " " << rule.op << " " << rule.var2;
            if (targetVarInvolvedAsVar1) {
                std::cerr << " (attempted to set " << targetVar << " = " << targetValue << ", but " << otherVar << " = " << otherValue << ")" << std::endl;
            } else {
                std::cerr << " (attempted to set " << targetVar << " = " << targetValue << ", but " << otherVar << " = " << otherValue << ")" << std::endl;
            }
            return false;
        }
    }
    
    return true; // All constraints are satisfied
}
