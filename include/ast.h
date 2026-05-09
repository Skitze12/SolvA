#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

// --- THE BASE CLASS ---
class Node {
protected:
    int nodeId;

public:
    Node();
    virtual ~Node() = default;

    /*
     * WHAT IT DOES: Generates the text needed to draw this node in a Graphviz diagram.
     * EXPECTS: Nothing.
     * RETURNS: A string formatted in the DOT language representing this specific node.
     */
    virtual std::string toDot() const = 0;
    
    /*
     * WHAT IT DOES: Returns the unique node ID for this node.
     * RETURNS: Integer node ID.
     */
    int getNodeId() const;
    
    /*
     * WHAT IT DOES: Checks if this node is a variable declaration.
     * RETURNS: True if VarDeclNode, false otherwise.
     */
    virtual bool isDeclaration() const { return false; }
    
    /*
     * WHAT IT DOES: Checks if this node is a constraint or assignment.
     * RETURNS: True if ConstraintNode or AssignmentNode, false otherwise.
     */
    virtual bool isConstraint() const { return false; }
};

// --- LEAF NODE (for displaying simple values) ---
class LeafNode : public Node {
private:
    std::string label;
    std::string color;

public:
    /*
     * EXPECTS: Label text and optional color.
     */
    LeafNode(std::string lbl, std::string col = "white");

    /*
     * RETURNS: A DOT string for a simple leaf node.
     */
    std::string toDot() const override;
};

// --- EOF NODE (terminal marker) ---
class EOFNode : public Node {
public:
    EOFNode();
    std::string toDot() const override;
};

// --- LINE NODE (intermediate grouping for statement + next line) ---
class LineNode : public Node {
private:
    Node* statement;
    Node* nextLine;

public:
    /*
     * EXPECTS: Nothing. Initialize empty, then use setStatement() and setNextLine().
     */
    LineNode();
    ~LineNode();

    /*
     * WHAT IT DOES: Sets the statement for this line.
     * EXPECTS: A pointer to a statement node.
     */
    void setStatement(Node* stmt);

    /*
     * WHAT IT DOES: Sets the next line node.
     * EXPECTS: A pointer to the next LineNode or EOFNode.
     */
    void setNextLine(Node* next);

    /*
     * RETURNS: A DOT string for this line and its recursive structure.
     */
    std::string toDot() const override;
};


// --- THE ASSIGNMENT NODE ---
// Represents a line like: Sindh = 1;
class AssignmentNode : public Node {
private:
    std::string varName;
    int value;
    bool isManual;  // Track if this is manual or machine input
    LeafNode* varNode;
    LeafNode* valueNode;
    LeafNode* typeNode;  // Shows "MANUAL" or "MACHINE"

public:
    /*
     * EXPECTS: The name of the variable, the integer assigned to it, and whether it's manual input.
     */
    AssignmentNode(std::string v, int val, bool manual = true);
    ~AssignmentNode();

    /*
     * RETURNS: A DOT string showing the assignment and its type (manual/machine).
     */
    std::string toDot() const override;
    
    /*
     * RETURNS: True (assignments are constraints).
     */
    bool isConstraint() const override { return true; }
    
    /*
     * WHAT IT DOES: Sets whether this assignment is manual or machine-generated.
     */
    void setManual(bool manual) { isManual = manual; }
};


// --- THE VARIABLE DECLARATION NODE ---
// Represents a line like: var Sindh in [1..3];
class VarDeclNode : public Node {
private:
    std::string varName;
    int minVal;
    int maxVal;
    LeafNode* nameNode;
    LeafNode* minNode;
    LeafNode* maxNode;

public:
    /*
     * EXPECTS: The name of the variable and its domain bounds [minVal..maxVal].
     */
    VarDeclNode(std::string v, int min_val, int max_val);
    ~VarDeclNode();

    /*
     * RETURNS: A DOT string like: node_0 [label="Var: Sindh in [1..3]"];
     */
    std::string toDot() const override;
    
    /*
     * RETURNS: True (var declarations are declarations).
     */
    bool isDeclaration() const override { return true; }
};


// --- THE CONSTRAINT NODE ---
// Represents a line like: constraint Sindh != Punjab;
class ConstraintNode : public Node {
private:
    std::string var1;
    std::string op;
    std::string var2;
    LeafNode* leftNode;
    LeafNode* opNode;
    LeafNode* rightNode;

public:
    /*
     * EXPECTS: Left variable, the operator, and the right variable.
     */
    ConstraintNode(std::string v1, std::string op, std::string v2);
    ~ConstraintNode();

    /*
     * RETURNS: A DOT string like: node_2 [label="Constraint: Sindh != Punjab"];
     */
    std::string toDot() const override;
    
    /*
     * RETURNS: True (constraints are constraints).
     */
    bool isConstraint() const override { return true; }
};

// --- THE ROOT PROGRAM NODE ---
// Represents the entire file. Has a single child: the first line.
class ProgramNode : public Node {
private:
    Node* firstLine;

public:
    ProgramNode();
    ~ProgramNode();

    /*
     * WHAT IT DOES: Sets the first line of the program.
     * EXPECTS: A pointer to a LineNode or EOFNode.
     * RETURNS: Nothing (void).
     */
    void setFirstLine(Node* line);

    /*
     * WHAT IT DOES: Generates DOT output for the entire program tree.
     * RETURNS: The complete, final DOT file string to be saved to a file.
     */
    std::string toDot() const override;
};

#endif  