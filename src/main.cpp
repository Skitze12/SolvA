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
extern std::vector<std::string> manuallyAssignedVars;

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
    //VALIDATE COMMAND LINE ARGUMENTS

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename.solva>" << endl;
        return 1;
    }

    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        cerr << "FILE ERROR: Could not open file '" << argv[1] << "'" << endl;
        cerr << "Possible causes:" << endl;
        cerr << "  - File does not exist" << endl;
        cerr << "  - Incorrect file path" << endl;
        cerr << "  - Permission denied" << endl;
        return 1;
    }
    
    yyin = inputFile;

    cout << "--- Starting SolvA Validation ---" << endl;
    cout << "Input file: " << argv[1] << endl << endl;

    
    //EXECUTE THE COMPILER PIPELINE
  
    int parseResult = yyparse();

    if (parseResult == 0 && astRoot != nullptr) {
        cout << "\n✓ SUCCESS: The provided CSP is syntactically, semantically, and logically valid!" << endl;
        
        // Print all manual assignments
        cout << "\n--- Manual Assignments (from file) ---" << endl;
        bool hasManual = false;
        for (const auto& var : manuallyAssignedVars) {
            if (symbolTable.hasValue(var)) {
                cout << "  " << var << " = " << symbolTable.getValue(var) << " (MANUAL)" << endl;
                hasManual = true;
            }
        }
        if (!hasManual) {
            cout << "  (No manual assignments)" << endl;
        }
        
        //handle "Machine Inputs" dynamically during parsing.
        
        if (manuallyAssignedVars.size() == symbolTable.getAllVariables().size()) {
            cout << "\n--- Final Solution ---" << endl;
            cout << "All variables have been assigned manually. No machine-generated solutions needed." << endl;
        }
        
        string dotCode = astRoot->toDot();
        saveDotToFile(dotCode, "output.dot");
        
        visualizeAst("output.dot");

    } else {
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
    
    fclose(inputFile);
    
    if (astRoot) {
        delete astRoot;
        astRoot = nullptr;
    }

    return parseResult;
}