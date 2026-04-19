#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <unordered_map>
#include <string>

class SymbolTable {
private:
    // The dictionary storing variable names and their current values.
    std::unordered_map<std::string, int> memory;

public:
    SymbolTable() = default;
    ~SymbolTable() = default;

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
};

#endif