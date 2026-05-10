#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <unordered_map>
#include <string>
#include <vector>


struct VariableDomain {
    int minValue;
    int maxValue;
};

class SymbolTable {
private:
    std::unordered_map<std::string, int> memory;
    
    std::unordered_map<std::string, VariableDomain> domains;

public:
    SymbolTable() = default;
    ~SymbolTable() = default;

    void registerVariable(const std::string& name, int minVal, int maxVal);


    void setValue(const std::string& name, int val);

 
    bool hasValue(const std::string& name) const;

    int getValue(const std::string& name) const;
    VariableDomain getDomain(const std::string& name) const;
    
 
    std::vector<std::string> getAllVariables() const;
  
    void clearAssignments();
    
    void unsetValue(const std::string& name);

    bool variableExists(const std::string& name) const;
    
    bool isValueInDomain(const std::string& name, int val) const;
    
 
    int getUnassignedCount() const;

    std::vector<std::string> getUnassignedVariables() const;
};

#endif