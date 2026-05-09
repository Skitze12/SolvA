# AST Visualizer Guide

## Overview

The SolvA compiler now includes an automatic AST (Abstract Syntax Tree) visualizer that generates visual representations of your parsed constraint satisfaction programs.

## Features

- **Automatic visualization**: When your program compiles successfully, the AST is automatically visualized
- **Multiple formats**: Generates both PNG and SVG formats
  - **PNG**: Easy-to-view raster image format
  - **SVG**: Scalable vector format, can be edited in design tools
- **Color-coded nodes**:
  - 🔴 **Red (Coral)**: Root Program node
  - 🔵 **Blue**: Variable declarations
  - 🟢 **Green**: Assignments
  - 🟡 **Yellow**: Constraints
- **Automatic viewer**: On Windows, PNG files automatically open after generation

## Requirements

To use the visualizer, you need **Graphviz** installed on your system:

### Installation

**Windows:**
```
Download from: https://graphviz.org/download/
Run the installer (choco install graphviz if using Chocolatey)
Add to PATH during installation (recommended)
```

**macOS:**
```
brew install graphviz
```

**Linux (Ubuntu/Debian):**
```
sudo apt-get install graphviz
```

**Linux (Fedora/RHEL):**
```
sudo dnf install graphviz
```

## Usage

### Standard Compilation with Visualization

```bash
make
./solva examples/nqueens.solva
```

On successful compilation, you'll see:
```
--- AST Visualization ---
Generating visualizations from: output.dot
Converting DOT to png...
Command: dot -Tpng "output.dot" -o "output.png"
✓ Successfully created: output.png
Converting DOT to svg...
Command: dot -Tsvg "output.dot" -o "output.svg"
✓ Successfully created: output.svg
```

### Output Files

After successful compilation, three files are created:

1. **output.dot** - Raw Graphviz format (text file)
2. **output.png** - PNG image of the AST tree
3. **output.svg** - SVG image of the AST tree

### Viewing the Visualization

**PNG (Windows):**
- Automatically opens in your default image viewer
- Can be manually opened with any image viewer

**PNG (macOS/Linux):**
- Open manually: `open output.png` (macOS) or `xdg-open output.png` (Linux)

**SVG (All Platforms):**
- Open in any web browser
- Can be edited with: Inkscape, Adobe Illustrator, or online editors
- Scalable to any size without quality loss

## Example Output

For a file like `examples/nqueens.solva`:

```solva
var Queen1 in [1..8];
var Queen2 in [1..8];
constraint Queen1 != Queen2;
```

The visualizer produces a tree showing:
```
        Program (root)
           |
        ┌──┼──┐
        |  |  |
      Var Var Con
    Queen1 Queen2 (Q1 != Q2)
```

## Troubleshooting

**Error: "Graphviz is not installed or 'dot' is not accessible"**

1. Verify Graphviz installation:
   ```bash
   dot -V
   ```
   
2. If not found, install from: https://graphviz.org/download/

3. Ensure 'dot' is in your system PATH:
   - Windows: Check System Environment Variables
   - macOS/Linux: Should be automatic after installation

**PNG file doesn't open automatically**

- This is expected on Linux/macOS
- Open manually or use the provided commands above
- Check file permissions: `ls -la output.png`

**SVG looks different in different viewers**

- This is normal; some viewers render SVG differently
- Try opening in multiple browsers or Inkscape for best results

## Advanced: Manual Conversion

If you want to generate other image formats:

```bash
# Convert DOT to PDF
dot -Tpdf output.dot -o output.pdf

# Convert DOT to EPS
dot -Teps output.dot -o output.eps

# Convert DOT to GIF
dot -Tgif output.dot -o output.gif
```

Supported formats: svg, png, jpg, pdf, eps, gif, and many more!

## Integration with Development Workflow

The visualizer helps with:
- **Debugging**: See exactly how the parser interpreted your code
- **Learning**: Understand the structure of CSP programs
- **Documentation**: Generate visual proof of correct parsing
- **Testing**: Verify AST structure matches expectations

## Customization

To modify visualization styling, edit [src/ast.cpp](src/ast.cpp):

```cpp
// In VarDeclNode::toDot():
oss << "node_" << nodeId << " [label=\"...\", shape=box, style=filled, fillcolor=lightblue];\n";
//                                                        ^^^^                     ^^^^^^^^^
//                                              shape options            color options
```

Available shapes: box, circle, ellipse, diamond, etc.
Available colors: lightblue, lightgreen, lightyellow, white, red, etc.

## Related Files

- [include/ast.h](include/ast.h) - AST node definitions
- [include/visualizer.h](include/visualizer.h) - Visualizer function declarations
- [src/ast.cpp](src/ast.cpp) - AST node implementations with DOT generation
- [src/visualizer.cpp](src/visualizer.cpp) - Graphviz conversion functions
- [src/main.cpp](src/main.cpp) - Integration with main compilation pipeline

---

Happy visualizing! 🎨
