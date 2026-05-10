#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>

/*
 Converts a DOT file to an image using Graphviz
 */
bool convertDotToImage(const std::string& dotFile, const std::string& outputFile, const std::string& format) {
    //dot -Tformat input.dot -o output.image
    std::ostringstream command;
    command << "dot -T" << format << " \"" << dotFile << "\" -o \"" << outputFile << "\"";
    
    std::cout << "Converting DOT to " << format << "..." << std::endl;
    std::cout << "Command: " << command.str() << std::endl;
    
    int result = system(command.str().c_str());
    
    if (result == 0) {
        std::cout << "✓ Successfully created: " << outputFile << std::endl;
        return true;
    } else {
        std::cerr << "✗ Error: Failed to convert DOT file to " << format << std::endl;
        std::cerr << "Make sure Graphviz is installed and 'dot' is in your PATH." << std::endl;
        return false;
    }
}

bool openImageViewer(const std::string& imageFile) {
    std::ifstream file(imageFile);
    if (!file.good()) {
        std::cerr << "Error: Image file not found: " << imageFile << std::endl;
        return false;
    }
    
    #ifdef _WIN32
        // Windows command
        std::ostringstream command;
        command << "start \"\" \"" << imageFile << "\"";
        int result = system(command.str().c_str());
    #elif __APPLE__
        // macOS command
        std::ostringstream command;
        command << "open \"" << imageFile << "\"";
        int result = system(command.str().c_str());
    #else
        // Linux command
        std::ostringstream command;
        command << "xdg-open \"" << imageFile << "\" &";
        int result = system(command.str().c_str());
    #endif
    
    if (result != 0) {
        std::cerr << "Warning: Could not automatically open image viewer." << std::endl;
        return false;
    }
    return true;
}

/*
 Generates PNG from a DOT file

 */
bool visualizeAstAsPNG(const std::string& dotFile) {
    std::string pngFile = dotFile.substr(0, dotFile.rfind(".dot")) + ".png";
    
    bool success = convertDotToImage(dotFile, pngFile, "png");
    
    if (success) {
        std::cout << "\nAttempting to open visualization..." << std::endl;
        openImageViewer(pngFile);
    }
    
    return success;
}

/*
Generates SVG visualization
 */
bool visualizeAstAsSVG(const std::string& dotFile) {
    std::string svgFile = dotFile.substr(0, dotFile.rfind(".dot")) + ".svg";
    
    bool success = convertDotToImage(dotFile, svgFile, "svg");
    
    if (success) {
        std::cout << "SVG file can be opened in any web browser." << std::endl;
    }
    
    return success;
}


 //Generates  PNG and SVG visualizations

 
bool visualizeAst(const std::string& dotFile) {
    std::ifstream file(dotFile);
    if (!file.good()) {
        std::cerr << "Error: DOT file not found: " << dotFile << std::endl;
        return false;
    }
    
    std::cout << "\n--- AST Visualization ---" << std::endl;
    std::cout << "Generating visualizations from: " << dotFile << std::endl;
    
    bool pngSuccess = visualizeAstAsPNG(dotFile);
    bool svgSuccess = visualizeAstAsSVG(dotFile);
    
    if (!pngSuccess && !svgSuccess) {
        std::cerr << "\nNote: Graphviz is not installed or 'dot' is not accessible." << std::endl;
        std::cerr << "Install Graphviz from: https://graphviz.org/download/" << std::endl;
        return false;
    }
    
    return true;
}
