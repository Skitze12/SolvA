%{
/*Everything between %{%} are all the libraries*/

#include <stdio.h>
#include <stdlib.h>


/* %% ... %% represents all the pattern -> actions are defined in this sections */
/*Pattern or the regext expression maps to -> the action, right now its just print statements */
/* [Pattern] {C Action code}


%}

/* Tell Flex we are only reading one file at a time */
%option noyywrap


%%

"//".* { printf("COMMENT\n"); /* Ignore single-line comments */ }
[ \t\n\r]+              { /* Ignore spaces, tabs, and newlines */ }

"var"                   { printf("TOKEN: KEYWORD_VAR\n"); }
"in"                    { printf("TOKEN: KEYWORD_IN\n"); }
"constraint"            { printf("TOKEN: KEYWORD_CONSTRAINT\n"); }

"=="|"!="|">"|"<"       { printf("TOKEN: OPERATOR (%s)\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]* { printf("TOKEN: ID (%s)\n", yytext); }
[0-9]+                  { printf("TOKEN: INT (%s)\n", yytext); }

"="|";"|"."             { printf("TOKEN: SYMBOL (%s)\n", yytext); }

.                       { printf("LEXICAL ERROR: Unrecognized character '%s'\n", yytext); }

%%

/* 
 Temp main functoin for testing rn
*/
int main(int argc, char **argv) {
    
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            printf("Error: Could not open %s\n", argv[1]);
            return 1;
        }
        yyin = file; // Tells Flex to read from this file
    } else {
        printf("Reading from standard input (Type Ctrl+Z to stop):\n");
    }

    //yylex() is the main function which looks at all of the above rules and creates the DFA in C in the file lex.yy.c 

    yylex();
    return 0;
}