%{
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "parser.hpp"  // Include Bison-generated header for token types

%}

/* Tell Flex we are only reading one file at a time */
%option noyywrap
%option yylineno

%%

"//".* { /* Ignore single-line comments */ }
[ \t\r]+              { /* Ignore spaces and tabs */ }
\n                    { /* Count lines */ }

"var"                   { return KEYWORD_VAR; }
"in"                    { return KEYWORD_IN; }
"constraint"            { return KEYWORD_CONSTRAINT; }
"abs"                   { return KEYWORD_ABS; }
"Manual Inputs"         { return KEYWORD_MANUAL; }
"Machine Inputs"        { return KEYWORD_MACHINE; }

"=="                    { yylval.str_val = strdup(yytext); return OPERATOR; }
"!="                    { yylval.str_val = strdup(yytext); return OPERATOR; }
">"                     { yylval.str_val = strdup(yytext); return OPERATOR; }
"<"                     { yylval.str_val = strdup(yytext); return OPERATOR; }
">="                    { yylval.str_val = strdup(yytext); return OPERATOR; }
"<="                    { yylval.str_val = strdup(yytext); return OPERATOR; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.str_val = strdup(yytext); return ID; }
[0-9]+                  { yylval.int_val = atoi(yytext); return INT; }

"="                     { return ASSIGN; }
";"                     { return SEMICOLON; }
":"                     { return ':'; }
"."                     { return '.'; }
"["                     { return '['; }
"]"                     { return ']'; }
"("                     { return '('; }
")"                     { return ')'; }
"-"                     { return '-'; }

.                       { 
    // Provide more informative error message for common mistakes
    if (yytext[0] == '"' || yytext[0] == '\'') {
        std::cerr << "LEXICAL ERROR at line " << yylineno << ": Strings are not supported. Use identifiers instead." << std::endl;
    } else if (yytext[0] == '#' || yytext[0] == '@') {
        std::cerr << "LEXICAL ERROR at line " << yylineno << ": Invalid character '" << yytext[0] << "'. Comments start with //." << std::endl;
    } else {
        std::cerr << "LEXICAL ERROR at line " << yylineno << ": Unrecognized character '" << yytext[0] << "'." << std::endl;
    }
    return 1;
}

%%