%{
#include <iostream>
#include <string>
#include "ast.h"
#include "logic_engine.h"
#include "symbol_table.h"

// Forward declaration for error handling and lexer
void yyerror(const char* s);
int yylex();

// Allow line number tracking for better error messages
extern int yylineno;

// Global State Instances
SymbolTable symbolTable;
LogicEngine logicEngine;
ProgramNode* astRoot = nullptr;
bool inMachineInputMode = false;  // Track which input mode we're in
bool hasManualInputsSection = false;  // Track if Manual Inputs section exists
std::vector<std::string> manuallyAssignedVars;  // Track manually assigned variables
%}

%union {
    int int_val;
    char* str_val;
    void* node_val;
}

%destructor { free($$); } <str_val>

/* Keywords */
%token KEYWORD_VAR KEYWORD_IN KEYWORD_CONSTRAINT KEYWORD_MANUAL KEYWORD_MACHINE KEYWORD_ABS

/* Symbols */
%token ASSIGN SEMICOLON

/* Typed Tokens (carrying data from Flex) */
%token <str_val> ID
%token <int_val> INT
%token <str_val> OPERATOR

%type <node_val> statement constraint_def var_decl assignment_stmt input_section line

%%

program : line
        {
            astRoot = new ProgramNode();
            astRoot->setFirstLine((Node*)$1);
            
            // VALIDATION: Check if Manual Inputs section exists but variables are unassigned
            // Only throw an error if the user didn't request Machine Inputs to solve the rest
            if (hasManualInputsSection && !inMachineInputMode) {
                std::vector<std::string> unassignedInManual = symbolTable.getUnassignedVariables();
                if (!unassignedInManual.empty()) {
                    std::cerr << "\nSEMANTIC ERROR: Manual Inputs section found but the following variables are not assigned:" << std::endl;
                    for (const auto& var : unassignedInManual) {
                        std::cerr << "  - " << var << std::endl;
                    }
                    YYERROR;
                }
            }
            
            // After parsing all manual inputs, find solution for remaining variables
            std::vector<std::string> unassignedVars;
            for (const auto& var : symbolTable.getAllVariables()) {
                if (!symbolTable.hasValue(var)) {
                    unassignedVars.push_back(var);
                }
            }
            
            if (!unassignedVars.empty()) {
                std::cout << "\nFinding machine-generated solutions for remaining variables..." << std::endl;
                if (logicEngine.findSolution(symbolTable)) {
                    std::cout << "✓ Machine found solution for:" << std::endl;
                    for (const auto& var : unassignedVars) {
                        if (symbolTable.hasValue(var)) {
                            std::cout << "  " << var << " = " << symbolTable.getValue(var) << " (MACHINE)" << std::endl;
                        }
                    }
                }
            }
        }
        ;

line : statement line
     {
         // Create a line node with the statement and next line
         LineNode* lineNode = new LineNode();
         lineNode->setStatement((Node*)$1);
         lineNode->setNextLine((Node*)$2);
         $$ = (void*)lineNode;
     }
     | input_section line
     {
         // Handle input section markers
         $$ = $2;  // Just continue with next line, section is for mode tracking
     }
     | /* empty - end of file */
     {
         // Terminate with EOF node
         $$ = (void*)(new EOFNode());
     }
     ;

statement : var_decl       { $$ = $1; }
          | constraint_def { $$ = $1; }
          | assignment_stmt { $$ = $1; }
          ;

input_section : KEYWORD_MANUAL ':'
              {
                  hasManualInputsSection = true;
                  inMachineInputMode = false;
                  std::cout << "Entering Manual Inputs section..." << std::endl;
              }
              | KEYWORD_MACHINE ':'
              {
                  inMachineInputMode = true;
                  std::cout << "Entering Machine Inputs section..." << std::endl;
              }
              ;

// Example: var Sindh in [1..3];
var_decl : KEYWORD_VAR ID KEYWORD_IN '[' INT '.' '.' INT ']' SEMICOLON
         {
             // ERROR: Check for duplicate variable declaration
             if (symbolTable.variableExists($2)) {
                 std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Variable '" << $2 
                           << "' has already been declared!" << std::endl;
                 YYERROR;
             }
             
             // ERROR: Check for invalid domain (min > max)
             if ($5 > $8) {
                 std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Invalid domain for '" << $2 
                           << "': min value (" << $5 << ") cannot be greater than max value (" << $8 << ")" << std::endl;
                 YYERROR;
             }
             
             // Register the variable's domain in the symbol table
             symbolTable.registerVariable($2, $5, $8);
             
             // Build AST Node
             $$ = (void*)(new VarDeclNode($2, $5, $8));
             
             // Memory Cleanup for the ID token
             free($2); 
         }
         ;

// Example: constraint Sindh != Punjab;
// Example: constraint abs(Col1 - Col2) != 1;
constraint_def : KEYWORD_CONSTRAINT ID OPERATOR ID SEMICOLON
               {
                   // ERROR: Check if left variable exists
                   if (!symbolTable.variableExists($2)) {
                       std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Variable '" << $2 
                                 << "' used in constraint but never declared!" << std::endl;
                       YYERROR;
                   }
                   
                   // ERROR: Check if right variable exists
                   if (!symbolTable.variableExists($4)) {
                       std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Variable '" << $4 
                                 << "' used in constraint but never declared!" << std::endl;
                       YYERROR;
                   }
                   
                   // Logic Trigger: Add rule to logic engine
                   logicEngine.addRule($2, $3, $4);
                   
                   // Build AST Node
                   $$ = (void*)(new ConstraintNode($2, $3, $4));
                   
                   // Memory Cleanup for string tokens
                   free($2); free($3); free($4);
               }
               | KEYWORD_CONSTRAINT KEYWORD_ABS '(' ID '-' ID ')' OPERATOR INT SEMICOLON
               {
                   // ERROR: Check if left variable exists
                   if (!symbolTable.variableExists($4)) {
                       std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Variable '" << $4 
                                 << "' used in constraint but never declared!" << std::endl;
                       YYERROR;
                   }
                   
                   // ERROR: Check if right variable exists
                   if (!symbolTable.variableExists($6)) {
                       std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Variable '" << $6 
                                 << "' used in constraint but never declared!" << std::endl;
                       YYERROR;
                   }
                   
                   // Logic Trigger: Add absolute difference rule to logic engine
                   logicEngine.addRule($4, $6, $8, $9);
                   
                   // Build AST Node
                   $$ = (void*)(new AbsConstraintNode($4, $6, $8, $9));
                   
                   // Memory Cleanup for string tokens
                   free($4); free($6); free($8);
               }
               ;

// Example: Col1 = 2;
assignment_stmt : ID ASSIGN INT SEMICOLON
                {
                    // ERROR: Check if variable is declared
                    if (!symbolTable.variableExists($1)) {
                        std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Variable '" << $1 
                                  << "' used in assignment but never declared!" << std::endl;
                        YYERROR;
                    }
                    
                    // ERROR: Check if value is within domain
                    if (!symbolTable.isValueInDomain($1, $3)) {
                        VariableDomain domain = symbolTable.getDomain($1);
                        std::cerr << "VALUE ERROR at line " << yylineno << ": Cannot assign " << $1 << " = " << $3 
                                  << " (value out of domain [" << domain.minValue << ".." << domain.maxValue << "])" << std::endl;
                        YYERROR;
                    }
                    
                    // ERROR: Check for duplicate assignment to same variable
                    if (symbolTable.hasValue($1)) {
                        std::cerr << "SEMANTIC ERROR at line " << yylineno << ": Variable '" << $1 
                                  << "' has already been assigned a value!" << std::endl;
                        YYERROR;
                    }
                    
                    // Validate the assignment against constraints
                    if (!logicEngine.isMoveValid($1, $3, symbolTable)) {
                        // Assignment violates a constraint
                        std::cerr << "CONSTRAINT VIOLATION at line " << yylineno << ": Cannot assign " 
                                  << $1 << " = " << $3 << " (violates existing constraints)" << std::endl;
                        YYERROR;
                    }
                    
                    // Assignment is valid, so register it in the symbol table
                    symbolTable.setValue($1, $3);
                    
                    // Track manual assignments
                    if (!inMachineInputMode) {
                        manuallyAssignedVars.push_back($1);
                    }
                    
                    // Build AST Node - marked based on input mode
                    AssignmentNode* assignNode = new AssignmentNode($1, $3, !inMachineInputMode);
                    $$ = (void*)assignNode;
                    
                    // Memory Cleanup
                    free($1);
                }
                ;

%%

// Error handler called by Bison when parsing fails
void yyerror(const char* s) {
    std::cerr << "SYNTAX ERROR at line " << yylineno << ": " << s << std::endl;
    std::cerr << "\nCommon syntax issues:" << std::endl;
    std::cerr << "  - Missing or misplaced semicolon (;)" << std::endl;
    std::cerr << "  - Invalid syntax in variable declaration: var NAME in [MIN..MAX];" << std::endl;
    std::cerr << "  - Invalid syntax in constraint: constraint VAR1 OP VAR2;" << std::endl;
    std::cerr << "  - Invalid syntax in assignment: VAR = VALUE;" << std::endl;
    std::cerr << "  - Missing colon (:) after 'Manual Inputs' or 'Machine Inputs'" << std::endl;
}
