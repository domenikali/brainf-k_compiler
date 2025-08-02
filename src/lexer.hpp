#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <string>
#include <vector>
#include "utils.hpp"
#include <stack>
#include <map>


/**
 * @brief This function lexes the Brainfuck source code and returns a vector of instructions.
 * The lexer reads the entire source file, parses the Brainfuck commands, and generates a list of instructions.
 * It also handles the first pass of optimization by merging consecutive commands and removing unnecessary ones.
 * @param options Compiler options structure that include optimization flags.
 * @param instructions_map A map to keep track of the number of each instruction type.
 * @return A vector of instructions representing the parsed Brainfuck code.
 */
std::vector<Instruction> lexer(CompilerOptions options,std::map<InstructionType,uint16_t> &instructions_map);



#endif