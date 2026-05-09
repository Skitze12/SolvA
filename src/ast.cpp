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

// --- TERMINAL NODE ---
TerminalNode::TerminalNode(std::string sym, std::string typ) : symbol(sym), type(typ) {}

std::string TerminalNode::toDot() const {
    std::ostringstream oss;
    std::string color = "lightyellow";  // Default color for terminals
    
    if (type == "keyword") {
        color = "lightblue";
    } else if (type == "operator") {
        color = "lightcoral";
    } else if (type == "number") {
        color = "lightgreen";
    } else if (type == "identifier") {
        color = "white";
    }
    
    oss << "node_" << nodeId << " [label=\"" << symbol << "\", shape=box, style=filled, fillcolor=" << color << ", fontsize=10];\n";
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
    // Terminals
    kwVarNode = new TerminalNode("var", "keyword");
    kwInNode = new TerminalNode("in", "keyword");
    bracketOpenNode = new TerminalNode("[", "symbol");
    bracketCloseNode = new TerminalNode("]", "symbol");
    dotDotNode = new TerminalNode("..", "symbol");
    semicolonNode = new TerminalNode(";", "symbol");
    
    // Values
    nameNode = new LeafNode(v, "white");
    minNode = new LeafNode(std::to_string(min_val), "lightgreen");
    maxNode = new LeafNode(std::to_string(max_val), "lightgreen");
}

VarDeclNode::~VarDeclNode() {
    delete kwVarNode;
    delete nameNode;
    delete kwInNode;
    delete bracketOpenNode;
    delete minNode;
    delete dotDotNode;
    delete maxNode;
    delete bracketCloseNode;
    delete semicolonNode;
}

std::string VarDeclNode::toDot() const {
    std::ostringstream oss;
    oss << "node_" << nodeId << " [label=\"VarDecl\", shape=box, style=filled, fillcolor=lightblue];\n";
    
    // Add all children in order
    oss << "  " << kwVarNode->toDot();
    oss << "  " << nameNode->toDot();
    oss << "  " << kwInNode->toDot();
    oss << "  " << bracketOpenNode->toDot();
    oss << "  " << minNode->toDot();
    oss << "  " << dotDotNode->toDot();
    oss << "  " << maxNode->toDot();
    oss << "  " << bracketCloseNode->toDot();
    oss << "  " << semicolonNode->toDot();
    
    // Add edges
    oss << "  node_" << nodeId << " -> node_" << kwVarNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << nameNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << kwInNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << bracketOpenNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << minNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << dotDotNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << maxNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << bracketCloseNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << semicolonNode->getNodeId() << ";\n";
    
    return oss.str();
}

// --- ASSIGNMENT NODE ---
AssignmentNode::AssignmentNode(std::string v, int val, bool manual) 
    : varName(v), value(val), isManual(manual) {
    varNode = new LeafNode(v, "white");
    valueNode = new LeafNode(std::to_string(val), "lightgreen");
    typeNode = new LeafNode(isManual ? "MANUAL" : "MACHINE", isManual ? "lightgreen" : "lightyellow");
    assignOpNode = new TerminalNode("=", "operator");
    semicolonNode = new TerminalNode(";", "symbol");
}

AssignmentNode::~AssignmentNode() {
    delete varNode;
    delete assignOpNode;
    delete valueNode;
    delete semicolonNode;
    delete typeNode;
}

std::string AssignmentNode::toDot() const {
    std::ostringstream oss;
    std::string boxColor = isManual ? "lightgreen" : "lightyellow";
    oss << "node_" << nodeId << " [label=\"Assignment\", shape=box, style=filled, fillcolor=" << boxColor << "];\n";
    
    // Add all children in order
    oss << "  " << varNode->toDot();
    oss << "  " << assignOpNode->toDot();
    oss << "  " << valueNode->toDot();
    oss << "  " << semicolonNode->toDot();
    oss << "  " << typeNode->toDot();
    
    // Add edges
    oss << "  node_" << nodeId << " -> node_" << varNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << assignOpNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << valueNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << semicolonNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << typeNode->getNodeId() << " [label=\"source\"];\n";
    
    return oss.str();
}

// --- CONSTRAINT NODE ---
ConstraintNode::ConstraintNode(std::string v1, std::string op_str, std::string v2) 
    : var1(v1), op(op_str), var2(v2) {
    kwConstraintNode = new TerminalNode("constraint", "keyword");
    leftNode = new LeafNode(v1, "white");
    opNode = new TerminalNode(op_str, "operator");
    rightNode = new LeafNode(v2, "white");
    semicolonNode = new TerminalNode(";", "symbol");
}

ConstraintNode::~ConstraintNode() {
    delete kwConstraintNode;
    delete leftNode;
    delete opNode;
    delete rightNode;
    delete semicolonNode;
}

std::string ConstraintNode::toDot() const {
    std::ostringstream oss;
    oss << "node_" << nodeId << " [label=\"Constraint\", shape=box, style=filled, fillcolor=lightyellow];\n";
    
    // Add all children in order
    oss << "  " << kwConstraintNode->toDot();
    oss << "  " << leftNode->toDot();
    oss << "  " << opNode->toDot();
    oss << "  " << rightNode->toDot();
    oss << "  " << semicolonNode->toDot();
    
    // Add edges
    oss << "  node_" << nodeId << " -> node_" << kwConstraintNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << leftNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << opNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << rightNode->getNodeId() << ";\n";
    oss << "  node_" << nodeId << " -> node_" << semicolonNode->getNodeId() << ";\n";
    
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
