#include "../include/ast.h"
#include <sstream>

// Static counter for generating unique node IDs in DOT output
static int nodeCounter = 0;

// --- BASE NODE CLASS ---
Node::Node() : nodeId(nodeCounter++) {}

int Node::getNodeId() const {
    return nodeId;
}

// --- LEAF NODE ---
LeafNode::LeafNode(std::string lbl, std::string col) : label(lbl), color(col) {}

std::string LeafNode::toDot() const {
    std::ostringstream oss;
    oss << "node_" << nodeId << " [label=\"" << label << "\", shape=ellipse, style=filled, fillcolor=" << color << "];\n";
    return oss.str();
}

// --- EOF NODE ---
EOFNode::EOFNode() {}

std::string EOFNode::toDot() const {
    std::ostringstream oss;
    oss << "node_" << nodeId << " [label=\"EOF\", shape=ellipse, style=filled, fillcolor=lightgray];\n";
    return oss.str();
}

// --- LINE NODE ---
LineNode::LineNode() : statement(nullptr), nextLine(nullptr) {}

LineNode::~LineNode() {
    // Don't delete children; they are managed elsewhere
}

void LineNode::setStatement(Node* stmt) {
    statement = stmt;
}

void LineNode::setNextLine(Node* next) {
    nextLine = next;
}

std::string LineNode::toDot() const {
    std::ostringstream oss;
    oss << "node_" << nodeId << " [label=\"Line\", shape=box, style=filled, fillcolor=\"#E0E0E0\"];\n";
    
    // Add statement
    if (statement) {
        oss << "  " << statement->toDot();
        oss << "  node_" << nodeId << " -> node_" << statement->getNodeId() << ";\n";
    }
    
    // Add next line recursively
    if (nextLine) {
        oss << "  " << nextLine->toDot();
        oss << "  node_" << nodeId << " -> node_" << nextLine->getNodeId() << ";\n";
    }
    
    return oss.str();
}

// --- VARIABLE DECLARATION NODE ---
VarDeclNode::VarDeclNode(std::string v, int min_val, int max_val) 
    : varName(v), minVal(min_val), maxVal(max_val) {
    nameNode = new LeafNode(v, "white");
    minNode = new LeafNode(std::to_string(min_val), "white");
    maxNode = new LeafNode(std::to_string(max_val), "white");
}

VarDeclNode::~VarDeclNode() {
    delete nameNode;
    delete minNode;
    delete maxNode;
}

std::string VarDeclNode::toDot() const {
    std::ostringstream oss;
    oss << "node_" << nodeId << " [label=\"Variable\", shape=box, style=filled, fillcolor=lightblue];\n";
    oss << "  " << nameNode->toDot();
    oss << "  " << minNode->toDot();
    oss << "  " << maxNode->toDot();
    oss << "  node_" << nodeId << " -> node_" << nameNode->getNodeId() << " [label=\"name\"];\n";
    oss << "  node_" << nodeId << " -> node_" << minNode->getNodeId() << " [label=\"min\"];\n";
    oss << "  node_" << nodeId << " -> node_" << maxNode->getNodeId() << " [label=\"max\"];\n";
    return oss.str();
}

// --- ASSIGNMENT NODE ---
AssignmentNode::AssignmentNode(std::string v, int val, bool manual) 
    : varName(v), value(val), isManual(manual) {
    varNode = new LeafNode(v, "white");
    valueNode = new LeafNode(std::to_string(val), "white");
    typeNode = new LeafNode(isManual ? "MANUAL" : "MACHINE", isManual ? "lightgreen" : "lightyellow");
}

AssignmentNode::~AssignmentNode() {
    delete varNode;
    delete valueNode;
    delete typeNode;
}

std::string AssignmentNode::toDot() const {
    std::ostringstream oss;
    std::string boxColor = isManual ? "lightgreen" : "lightyellow";
    oss << "node_" << nodeId << " [label=\"Assignment\", shape=box, style=filled, fillcolor=" << boxColor << "];\n";
    oss << "  " << varNode->toDot();
    oss << "  " << valueNode->toDot();
    oss << "  " << typeNode->toDot();
    oss << "  node_" << nodeId << " -> node_" << varNode->getNodeId() << " [label=\"var\"];\n";
    oss << "  node_" << nodeId << " -> node_" << valueNode->getNodeId() << " [label=\"value\"];\n";
    oss << "  node_" << nodeId << " -> node_" << typeNode->getNodeId() << " [label=\"source\"];\n";
    return oss.str();
}

// --- CONSTRAINT NODE ---
ConstraintNode::ConstraintNode(std::string v1, std::string op_str, std::string v2) 
    : var1(v1), op(op_str), var2(v2) {
    leftNode = new LeafNode(v1, "white");
    opNode = new LeafNode(op_str, "white");
    rightNode = new LeafNode(v2, "white");
}

ConstraintNode::~ConstraintNode() {
    delete leftNode;
    delete opNode;
    delete rightNode;
}

std::string ConstraintNode::toDot() const {
    std::ostringstream oss;
    oss << "node_" << nodeId << " [label=\"Constraint\", shape=box, style=filled, fillcolor=lightyellow];\n";
    oss << "  " << leftNode->toDot();
    oss << "  " << opNode->toDot();
    oss << "  " << rightNode->toDot();
    oss << "  node_" << nodeId << " -> node_" << leftNode->getNodeId() << " [label=\"left\"];\n";
    oss << "  node_" << nodeId << " -> node_" << opNode->getNodeId() << " [label=\"operator\"];\n";
    oss << "  node_" << nodeId << " -> node_" << rightNode->getNodeId() << " [label=\"right\"];\n";
    return oss.str();
}

// --- PROGRAM NODE ---
ProgramNode::ProgramNode() : firstLine(nullptr) {}

ProgramNode::~ProgramNode() {
    // firstLine and its children are managed by the recursive structure
}

void ProgramNode::setFirstLine(Node* line) {
    firstLine = line;
}

std::string ProgramNode::toDot() const {
    std::ostringstream oss;
    oss << "digraph Program {\n";
    oss << "  rankdir=TB;\n";
    oss << "  graph [bgcolor=white];\n";
    oss << "  node [fontname=\"Arial\"];\n";
    oss << "  node_" << nodeId << " [label=\"Program\", shape=ellipse, style=filled, fillcolor=lightcoral, fontsize=14, fontweight=bold];\n";
    
    // Add first line and its recursive structure
    if (firstLine) {
        oss << "  " << firstLine->toDot();
        oss << "  node_" << nodeId << " -> node_" << firstLine->getNodeId() << ";\n";
    }
    
    oss << "}\n";
    return oss.str();
}
