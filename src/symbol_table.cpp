#include "../include/symbol_table.h"
#include <stdexcept>

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
