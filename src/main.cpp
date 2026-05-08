#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include "ast.h"

using namespace std;

// --- EXTERNAL FLEX/BISON VARIABLES ---
extern int yyparse();
extern FILE* yyin;
extern ProgramNode* astRoot;

// Helper function to save DOT output to a file
void saveDotToFile(const std::string& dotCode, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << dotCode;
        file.close();
        std::cout << "Parse tree saved to " << filename << std::endl;
    } else {
        std::cerr << "Error: Could not open " << filename << " for writing." << std::endl;
    }
}

int main(int argc, char** argv) {
    /*
     * STEP 1: VALIDATE COMMAND LINE ARGUMENTS
     * WHAT IT DOES: Ensures the user actually provided a file to compile.
     * EXPECTS: argc to be at least 2 (e.g., `./solva input.solva`).
     */
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename.solva>" << endl;
        return 1;
    }

    /*
     * STEP 2: FILE I/O
     * WHAT IT DOES: Opens the target .solva file and connects it to the Lexer.
     * EXPECTS: A valid file path.
     */
    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }
    
    // Tell Flex to read from this file instead of the terminal
    yyin = inputFile;

    cout << "--- Starting SolvA Validation ---" << endl;
    cout << "Input file: " << argv[1] << endl;

    /*
     * STEP 3: EXECUTE THE COMPILER PIPELINE
     * WHAT IT DOES: Runs the Lexer, Parser, and Logic Engine simultaneously (SDT).
     * EXPECTS: The rules defined in parser.y and logic_engine.cpp to handle the work.
     */
    int parseResult = yyparse();

    /*
     * STEP 4: HANDLE THE RESULTS
     * WHAT IT DOES: Provides final output and triggers AST visualization on success.
     */
    if (parseResult == 0 && astRoot != nullptr) {
        cout << "\n✓ SUCCESS: The provided CSP solution is syntactically and logically valid!" << endl;
        
        // Generate and save the AST visualization
        string dotCode = astRoot->toDot();
        saveDotToFile(dotCode, "output.dot");
        
        cout << "AST visualization generated successfully." << endl;

    } else {
        // Note: The specific logic error message will have already been printed 
        // by the yyerror() function inside parser.y before reaching this point.
        cout << "\n✗ FAILED: Validation aborted due to logic/syntax errors." << endl;
    }

    /*
     * STEP 5: CLEANUP
     * WHAT IT DOES: Closes file handles and frees memory to prevent leaks.
     */
    fclose(inputFile);
    
    // Clean up AST memory if allocated
    if (astRoot) {
        delete astRoot;
        astRoot = nullptr;
    }

    return parseResult;
}