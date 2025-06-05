#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "utils.hpp"
#include "architecture_interface.cpp"
#include <string.h>
#include <stack>
#include <vector>

FILE* debug_file(std::string file_name);
void debug(instructions_list instructions, CompilerOptions options);

#endif