#include "../include/ast.h"
#include <sstream>

// Static counter for generating unique node IDs in DOT output
static int nodeCounter = 0;

// --- VARIABLE DECLARATION NODE ---
VarDeclNode::VarDeclNode(std::string v, int min_val, int max_val) 
    : varName(v), minVal(min_val), maxVal(max_val) {}

std::string VarDeclNode::toDot() const {
    std::ostringstream oss;
    int nodeId = nodeCounter++;
    oss << "node_" << nodeId << " [label=\"Var: " << varName << " in [" << minVal << ".." << maxVal << "]\"];\n";
    return oss.str();
}

// --- ASSIGNMENT NODE ---
AssignmentNode::AssignmentNode(std::string v, int val) 
    : varName(v), value(val) {}

std::string AssignmentNode::toDot() const {
    std::ostringstream oss;
    int nodeId = nodeCounter++;
    oss << "node_" << nodeId << " [label=\"Assign: " << varName << " = " << value << "\"];\n";
    return oss.str();
}

// --- CONSTRAINT NODE ---
ConstraintNode::ConstraintNode(std::string v1, std::string op_str, std::string v2) 
    : var1(v1), op(op_str), var2(v2) {}

std::string ConstraintNode::toDot() const {
    std::ostringstream oss;
    int nodeId = nodeCounter++;
    oss << "node_" << nodeId << " [label=\"Constraint: " << var1 << " " << op << " " << var2 << "\"];\n";
    return oss.str();
}

// --- PROGRAM NODE ---
ProgramNode::~ProgramNode() {
    for (auto child : children) {
        delete child;
    }
}

void ProgramNode::addChild(Node* child) {
    children.push_back(child);
}

std::string ProgramNode::toDot() const {
    std::ostringstream oss;
    oss << "digraph Program {\n";
    oss << "  rankdir=TB;\n";
    
    for (const auto& child : children) {
        oss << "  " << child->toDot();
    }
    
    oss << "}\n";
    return oss.str();
}
