#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>

bool visualizeAstAsPNG(const std::string& dotFile);
bool visualizeAstAsSVG(const std::string& dotFile);


bool visualizeAst(const std::string& dotFile);

bool convertDotToImage(const std::string& dotFile, const std::string& outputFile, const std::string& format);

bool openImageViewer(const std::string& imageFile);

#endif  // VISUALIZER_H
