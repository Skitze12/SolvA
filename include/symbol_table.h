#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <unordered_map>
#include <string>
#include <vector>

// Structure to hold variable domain information
struct VariableDomain {
    int minValue;
    int maxValue;
};

class SymbolTable {
private:
    // The dictionary storing variable names and their current values.
    std::unordered_map<std::string, int> memory;
    
    // The dictionary storing variable domains (min/max bounds)
    std::unordered_map<std::string, VariableDomain> domains;

public:
    SymbolTable() = default;
    ~SymbolTable() = default;

    /*
     * WHAT IT DOES: Registers a variable with its domain bounds.
     * EXPECTS: The variable's name and its min/max bounds.
     * RETURNS: Nothing (void).
     */
    void registerVariable(const std::string& name, int minVal, int maxVal);

    /*
     * WHAT IT DOES: Saves a new assignment into memory (e.g., Sindh = 1).
     * EXPECTS: The variable's name (string) and its assigned value (integer).
     * RETURNS: Nothing (void).
     */
    void setValue(const std::string& name, int val);

    /*
     * WHAT IT DOES: Checks if a variable has already been assigned a number.
     * EXPECTS: The variable's name (string).
     * RETURNS: True if it has a value, False if it is still empty/unassigned.
     */
    bool hasValue(const std::string& name) const;

    /*
     * WHAT IT DOES: Retrieves the current value of a variable.
     * EXPECTS: The variable's name (string).
     * RETURNS: The integer value currently held by that variable.
     */
    int getValue(const std::string& name) const;
    
    /*
     * WHAT IT DOES: Returns the domain bounds for a variable.
     * EXPECTS: The variable's name (string).
     * RETURNS: A VariableDomain struct with minValue and maxValue.
     */
    VariableDomain getDomain(const std::string& name) const;
    
    /*
     * WHAT IT DOES: Returns all registered variables.
     * RETURNS: Vector of variable names.
     */
    std::vector<std::string> getAllVariables() const;
    
    /*
     * WHAT IT DOES: Clears all assignments (but keeps domain info).
     * RETURNS: Nothing (void).
     */
    void clearAssignments();
    
    /*
     * WHAT IT DOES: Removes the assignment for a single variable for backtracking.
     * EXPECTS: The variable's name (string).
     * RETURNS: Nothing (void).
     */
    void unsetValue(const std::string& name);
    
    /*
     * WHAT IT DOES: Checks if a variable has been declared.
     * EXPECTS: The variable's name (string).
     * RETURNS: True if variable exists, false otherwise.
     */
    bool variableExists(const std::string& name) const;
    
    /*
     * WHAT IT DOES: Validates if a value is within a variable's domain.
     * EXPECTS: Variable name and the value to check.
     * RETURNS: True if value is in [minValue..maxValue], false otherwise.
     */
    bool isValueInDomain(const std::string& name, int val) const;
    
    /*
     * WHAT IT DOES: Returns the count of unassigned variables.
     * RETURNS: Integer count of variables without values.
     */
    int getUnassignedCount() const;
    
    /*
     * WHAT IT DOES: Returns list of all unassigned variables.
     * RETURNS: Vector of variable names without values.
     */
    std::vector<std::string> getUnassignedVariables() const;
};

#endif