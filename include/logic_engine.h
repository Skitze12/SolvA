#ifndef LOGIC_ENGINE_H
#define LOGIC_ENGINE_H

#include "symbol_table.h"
#include <vector>
#include <string>

struct Constraint {
    bool isAbs; // true if this is an absolute difference rule
    std::string var1;
    std::string op;
    std::string var2; // Used if isAbs == false
    int intVal;       // Used if isAbs == true
};

class LogicEngine {
private:
    std::vector<Constraint> rules;
  
    bool solve(size_t varIndex, const std::vector<std::string>& variables, SymbolTable& st) const;

public:
    LogicEngine() = default;
    ~LogicEngine() = default;

    void addRule(const std::string& v1, const std::string& op, const std::string& v2);

    void addRule(const std::string& v1, const std::string& v2, const std::string& op, int intVal);


    bool isMoveValid(const std::string& targetVar, int targetValue, const SymbolTable& st) const;
    
 
    bool findSolution(SymbolTable& st) const;
};

#endif