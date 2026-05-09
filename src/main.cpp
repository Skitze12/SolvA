#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include "ast.h"
#include "visualizer.h"
#include "logic_engine.h"
#include "symbol_table.h"

using namespace std;

// --- EXTERNAL FLEX/BISON VARIABLES ---
extern int yyparse();
extern FILE* yyin;
extern ProgramNode* astRoot;
extern LogicEngine logicEngine;
extern SymbolTable symbolTable;

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
        cerr << "FILE ERROR: Could not open file '" << argv[1] << "'" << endl;
        cerr << "Possible causes:" << endl;
        cerr << "  - File does not exist" << endl;
        cerr << "  - Incorrect file path" << endl;
        cerr << "  - Permission denied" << endl;
        return 1;
    }
    
    // Tell Flex to read from this file instead of the terminal
    yyin = inputFile;

    cout << "--- Starting SolvA Validation ---" << endl;
    cout << "Input file: " << argv[1] << endl << endl;

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
        cout << "\n✓ SUCCESS: The provided CSP is syntactically, semantically, and logically valid!" << endl;
        
        // Print all manual assignments
        cout << "\n--- Manual Assignments (from file) ---" << endl;
        bool hasManual = false;
        for (const auto& var : symbolTable.getAllVariables()) {
            if (symbolTable.hasValue(var)) {
                cout << "  " << var << " = " << symbolTable.getValue(var) << " (MANUAL)" << endl;
                hasManual = true;
            }
        }
        if (!hasManual) {
            cout << "  (No manual assignments)" << endl;
        }
        
        // Attempt to find a valid solution using constraint solving for unassigned variables
        auto unassignedVars = symbolTable.getUnassignedVariables();
        
        if (!unassignedVars.empty()) {
            cout << "\n--- Machine-Generated Solutions ---" << endl;
            if (logicEngine.findSolution(symbolTable)) {
                for (const auto& var : unassignedVars) {
                    if (symbolTable.hasValue(var)) {
                        cout << "  " << var << " = " << symbolTable.getValue(var) << " (MACHINE)" << endl;
                    }
                }
            } else {
                cout << "✗ Machine could not find valid assignments for remaining variables." << endl;
            }
        } else {
            cout << "\n--- Final Solution ---" << endl;
            cout << "All variables have been assigned manually. No machine-generated solutions needed." << endl;
        }
        
        // Generate and save the AST visualization
        string dotCode = astRoot->toDot();
        saveDotToFile(dotCode, "output.dot");
        
        // Visualize the AST
        visualizeAst("output.dot");

    } else {
        // Note: Specific error messages will have already been printed by
        // the yyerror() function and semantic validation checks in parser.y
        cout << "\n✗ FAILED: Validation aborted due to errors." << endl;
        cout << "\nError Summary:" << endl;
        cout << "  - Check the error messages above for details" << endl;
        cout << "  - Common issues:" << endl;
        cout << "    * Missing semicolons after statements" << endl;
        cout << "    * Variables used but not declared" << endl;
        cout << "    * Duplicate variable declarations" << endl;
        cout << "    * Assignment values outside domain bounds" << endl;
        cout << "    * Constraint violations (e.g., Col1 = 2 when constraint says Col1 != 2)" << endl;
        cout << "    * Invalid domain specification (min > max)" << endl;
        cout << "    * Unassigned variables in Manual Inputs section" << endl;
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