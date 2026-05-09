#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>

/*
 * WHAT IT DOES: Generates PNG visualization from a DOT file and opens it
 * EXPECTS: Path to the DOT file
 * RETURNS: True if successful, false otherwise
 */
bool visualizeAstAsPNG(const std::string& dotFile);

/*
 * WHAT IT DOES: Generates SVG visualization from a DOT file
 * EXPECTS: Path to the DOT file
 * RETURNS: True if successful, false otherwise
 */
bool visualizeAstAsSVG(const std::string& dotFile);

/*
 * WHAT IT DOES: Generates both PNG and SVG visualizations
 * EXPECTS: Path to the DOT file
 * RETURNS: True if at least one format succeeds, false if both fail
 */
bool visualizeAst(const std::string& dotFile);

/*
 * WHAT IT DOES: Converts a DOT file to an image using Graphviz
 * EXPECTS: Input DOT filename, output image filename, and format (png, svg, pdf, etc.)
 * RETURNS: True if successful, false otherwise
 */
bool convertDotToImage(const std::string& dotFile, const std::string& outputFile, const std::string& format);

/*
 * WHAT IT DOES: Opens an image file with the default viewer
 * EXPECTS: Path to the image file
 * RETURNS: True if successful, false otherwise
 */
bool openImageViewer(const std::string& imageFile);

#endif  // VISUALIZER_H
