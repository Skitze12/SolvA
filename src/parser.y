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
    char* str_val;
    void* node_val;
}

%destructor { free($$); } <str_val>

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
                   astRoot->addChild((Node*)$2); 
               }
           }
           | statement 
           { 
               astRoot = new ProgramNode(); 
               if ($1 != nullptr) {
                   astRoot->addChild((Node*)$1); 
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
             // 1. Build AST Node
             $$ = (void*)(new VarDeclNode($2, $5, $8));
             
             // 2. Memory Cleanup for the ID token
             free($2); 
         }
         ;

// Example: constraint Sindh != Punjab;
constraint_def : KEYWORD_CONSTRAINT ID OPERATOR ID SEMICOLON
               {
                   // 1. Logic Trigger: Add rule to logic engine
                   logicEngine.addRule($2, $3, $4);
                   
                   // 2. Build AST Node
                   $$ = (void*)(new ConstraintNode($2, $3, $4));
                   
                   // 3. Memory Cleanup for string tokens
                   free($2); free($3); free($4);
               }
               ;

// Example: Sindh = 1;
assignment : ID ASSIGN INT SEMICOLON
           {
               // 1. Validate the move against constraints
               if (!logicEngine.isMoveValid($1, $3, symbolTable)) {
                   yyerror("Constraint violation detected during assignment");
                   free($1);
                   YYABORT;  // Stop parsing
               }
               
               // 2. Update symbol table
               symbolTable.setValue($1, $3);
               
               // 3. Build AST Node
               $$ = (void*)(new AssignmentNode($1, $3));
               
               // 4. Memory Cleanup
               free($1);
           }
           ;

%%

// Error handler called by Bison when parsing fails
void yyerror(const char* s) {
    std::cerr << "SYNTAX/LOGIC ERROR at line " << yylineno << ": " << s << std::endl;
}
