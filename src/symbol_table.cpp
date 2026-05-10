#include "../include/symbol_table.h"
#include <stdexcept>

void SymbolTable::registerVariable(const std::string& name, int minVal, int maxVal) {
    domains[name] = {minVal, maxVal};
}

void SymbolTable::setValue(const std::string& name, int val) {
    memory[name] = val;
}

bool SymbolTable::hasValue(const std::string& name) const {
    return memory.find(name) != memory.end();
}

int SymbolTable::getValue(const std::string& name) const {
    auto it = memory.find(name);
    if (it == memory.end()) {
        throw std::runtime_error("Variable '" + name + "' has no value assigned yet.");
    }
    return it->second;
}

VariableDomain SymbolTable::getDomain(const std::string& name) const {
    auto it = domains.find(name);
    if (it == domains.end()) {
        throw std::runtime_error("Variable '" + name + "' has no domain registered.");
    }
    return it->second;
}

std::vector<std::string> SymbolTable::getAllVariables() const {
    std::vector<std::string> vars;
    for (const auto& pair : domains) {
        vars.push_back(pair.first);
    }
    return vars;
}

void SymbolTable::clearAssignments() {
    memory.clear();
}

void SymbolTable::unsetValue(const std::string& name) {
    auto it = memory.find(name);
    if (it != memory.end()) {
        memory.erase(it);
    }
}

bool SymbolTable::variableExists(const std::string& name) const {
    return domains.find(name) != domains.end();
}

bool SymbolTable::isValueInDomain(const std::string& name, int val) const {
    auto it = domains.find(name);
    if (it == domains.end()) {
        return false;
    }
    return val >= it->second.minValue && val <= it->second.maxValue;
}

int SymbolTable::getUnassignedCount() const {
    int count = 0;
    for (const auto& pair : domains) {
        if (memory.find(pair.first) == memory.end()) {
            count++;
        }
    }
    return count;
}

std::vector<std::string> SymbolTable::getUnassignedVariables() const {
    std::vector<std::string> unassigned;
    for (const auto& pair : domains) {
        if (memory.find(pair.first) == memory.end()) {
            unassigned.push_back(pair.first);
        }
    }
    return unassigned;
}
