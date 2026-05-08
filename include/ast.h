#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

// --- THE BASE CLASS ---
class Node {
public:
    virtual ~Node() = default;

    /*
     * WHAT IT DOES: Generates the text needed to draw this node in a Graphviz diagram.
     * EXPECTS: Nothing.
     * RETURNS: A string formatted in the DOT language representing this specific node.
     */
    virtual std::string toDot() const = 0;
};


// --- THE ASSIGNMENT NODE ---
// Represents a line like: Sindh = 1;
class AssignmentNode : public Node {
private:
    std::string varName;
    int value;

public:
    /*
     * EXPECTS: The name of the variable and the integer assigned to it.
     */
    AssignmentNode(std::string v, int val);

    /*
     * RETURNS: A DOT string like: node_1 [label="Assign: Sindh = 1"];
     */
    std::string toDot() const override;
};


// --- THE VARIABLE DECLARATION NODE ---
// Represents a line like: var Sindh in [1..3];
class VarDeclNode : public Node {
private:
    std::string varName;
    int minVal;
    int maxVal;

public:
    /*
     * EXPECTS: The name of the variable and its domain bounds [minVal..maxVal].
     */
    VarDeclNode(std::string v, int min_val, int max_val);

    /*
     * RETURNS: A DOT string like: node_0 [label="Var: Sindh in [1..3]"];
     */
    std::string toDot() const override;
};


// --- THE CONSTRAINT NODE ---
// Represents a line like: constraint Sindh != Punjab;
class ConstraintNode : public Node {
private:
    std::string var1;
    std::string op;
    std::string var2;

public:
    /*
     * EXPECTS: Left variable, the operator, and the right variable.
     */
    ConstraintNode(std::string v1, std::string op, std::string v2);

    /*
     * RETURNS: A DOT string like: node_2 [label="Constraint: Sindh != Punjab"];
     */
    std::string toDot() const override;
};

// --- THE ROOT PROGRAM NODE ---
// Represents the entire file. Holds all other nodes as children.
class ProgramNode : public Node {
private:
    std::vector<Node*> children;

public:
    ProgramNode() = default;
    ~ProgramNode();

    /*
     * WHAT IT DOES: Attaches a new assignment or constraint node to the main tree.
     * EXPECTS: A pointer to the newly created node.
     * RETURNS: Nothing (void).
     */
    void addChild(Node* child);

    /*
     * WHAT IT DOES: Loops through all children and combines their DOT strings.
     * RETURNS: The complete, final DOT file string to be saved to a file.
     */
    std::string toDot() const override;
};

#endif  