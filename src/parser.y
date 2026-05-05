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
%}

%union {
    int int_val;
    std::string* str_val;
    Node* node_val;
}

%destructor { delete $$; } <str_val>
%destructor { delete $$; } <node_val>

/* Keywords */
%token KEYWORD_VAR KEYWORD_IN KEYWORD_CONSTRAINT

/* Symbols */
%token ASSIGN SEMICOLON

/* Typed Tokens (carrying data from Flex) */
%token <str_val> ID
%token <int_val> INT
%token <str_val> OPERATOR

%type <node_val> statement assignment constraint_def var_decl

%%

program : statements { /* Parsing complete */ }
        ;

statements : statements statement 
           { 
               // Avoid adding nullptrs to the tree if var_decl is temporarily returning null
               if ($2 != nullptr) {
                   astRoot->addChild($2); 
               }
           }
           | statement 
           { 
               astRoot = new ProgramNode(); 
               if ($1 != nullptr) {
                   astRoot->addChild($1); 
               }
           }
           ;

statement : var_decl       { $$ = $1; }
          | constraint_def { $$ = $1; }
          | assignment     { $$ = $1; }
          ;

// Example: var Sindh in [1..3];
var_decl : KEYWORD_VAR ID KEYWORD_IN '[' INT '.' '.' INT ']' SEMICOLON
         {
             // 1. Logic Hook: Tell the logic engine about the min/max bounds
             // TODO(Team): Uncomment once addDomain is implemented in LogicEngine
             // logicEngine.addDomain(*$2, $5, $8);
             
             // 2. Build AST Node
             // TODO(Team): Uncomment once VarDeclNode is implemented in ast.h
             // $$ = new VarDeclNode(*$2, $5, $8);
             
             $$ = nullptr; // Temporary placeholder to ensure it compiles right now
             
             // 3. Memory Cleanup for the ID token
             delete $2; 
         }
         ;

// Example: constraint Sindh != Punjab;
constraint_def : KEYWORD_CONSTRAINT ID OPERATOR ID SEMICOLON
               {
                   // 1. Logic Trigger: Add rule to logic engine
                   logicEngine.addRule(*$2, *$3, *$4);
                   
                   // 2. Build AST Node
                   $$ = new ConstraintNode(*$2, *$3, *$4);
                   
                   // 3. Memory Cleanup for string tokens
                   delete $2; delete $3; delete $4;
               }
               ;

// Example: Sindh = 1;
assignment : ID ASSIGN INT SEMICOLON
           {
               // 1. VALIDATION TRIGGER: Check the move against stored constraints
               if (!logicEngine.isMoveValid(*$1, $3, symbolTable)) {
                   // Logic Trigger Failed! Throw a syntax-level error.
                   yyerror("Logic conflict detected! Invalid assignment.");
                   delete $1; // Clean up before aborting
                   YYABORT;   // Immediately halt the parsing process
               }
               
               // 2. If valid, update the global state table
               symbolTable.setValue(*$1, $3);
               
               // 3. Build AST Node
               $$ = new AssignmentNode(*$1, $3);
               
               // 4. Memory Cleanup
               delete $1;
           }
           ;

%%

void yyerror(const char* s) {
    // Print the error with the exact line number
    std::cerr << "Syntax/Logic Error at line " << yylineno << ": " << s << std::endl;
    
    // Clean up the global AST to prevent memory leaks if the parser crashed
    if (astRoot != nullptr) {
        delete astRoot;
        astRoot = nullptr;
    }
}
