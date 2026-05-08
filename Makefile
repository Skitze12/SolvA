CXX = g++
FLEX = flex
BISON = bison
CXXFLAGS = -std=c++11 -Wall -Wextra -I./include

# Source files
SOURCES = src/main.cpp src/ast.cpp src/logic_engine.cpp src/symbol_table.cpp
LEXER_SOURCE = src/lexer.l
PARSER_SOURCE = src/parser.y

# Generated files
LEXER_GEN = src/lex.yy.c
PARSER_GEN = src/parser.cpp
PARSER_HEADER = src/parser.hpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o) src/lex.yy.o src/parser.o

# Output executable
TARGET = solva

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile Flex lexer
$(LEXER_GEN): $(LEXER_SOURCE)
	$(FLEX) -o $@ $<

# Compile Bison parser
$(PARSER_GEN) $(PARSER_HEADER): $(PARSER_SOURCE)
	$(BISON) -d -o $(PARSER_GEN) $<

# Compile C++ source files
src/%.o: src/%.cpp $(PARSER_HEADER)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile generated Flex lexer
src/lex.yy.o: $(LEXER_GEN)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile generated Bison parser
src/parser.o: $(PARSER_GEN)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJECTS) $(LEXER_GEN) $(PARSER_GEN) $(PARSER_HEADER) output.dot

run: $(TARGET)
	./$(TARGET) examples/nqueens.solva

test-nqueens: $(TARGET)
	./$(TARGET) examples/nqueens.solva

test-sudoku: $(TARGET)
	./$(TARGET) examples/sudoku.solva

test-simple: $(TARGET)
	./$(TARGET) testing/testing.solva

.PHONY: all clean run test-nqueens test-sudoku test-simple
