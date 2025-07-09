#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <sys/utsname.h>
#include <cstdint>
#include <vector>
#include "architecture_interface.hpp"
#include "comp_arch/x86.hpp"
#include "comp_arch/arm32.hpp"
#include "jit_arch/x86_jit.hpp"

enum class CompilerArch {
  X86_A,
  X86_64_A,
  ARM32_A,
  ARM64_A,
  RISCV_A,
  DLX_A,
  UNKNOWN
};

extern CompilerArch system_arch;

struct CompilerOptions{
  std::string source_file_name="";
  std::string output_file_name="";
  bool optimize = true; // Optimization flag
  bool debug = false; // Debugging flag
  bool verbose = false; // Verbose output flag
  uint64_t max_cycles = 0; // Maximum cycles flag
  uint64_t max_memory = 0; // Maximum memory flag
  CompilerArch target_arch=CompilerArch::UNKNOWN; // Default target architecture
  bool jit = false; // Just-In-Time compilation flag
};
typedef struct Compiler_Options Compiler_Options;

CompilerArch getTargetArch(const std::string &arch);

void getSystemArch();

FILE * fileRead(const char *filename);

FILE * fileWrite(const char *filename);

void verbose(CompilerOptions options, const std::string &message);

enum InstructionType{
  ADD = '+',
  SUB = '-',
  INC = '>',    
  DEC = '<',    
  INPUT = ',',  
  OUTPUT = '.', 
  BNEQ = ']',  
  BEQZ = '[',  
  UNKNOWN = '?' // Unknown instruction 
};
typedef enum InstructionType InstructionType;

typedef struct{
  InstructionType type;
  uint32_t extra; // Extra data for the instruction, e.g., times to repeat or branch adress
}Instruction;


//typedef std::pair<InstructionType, uint32_t> InstructionPair;

typedef std::vector<Instruction> instructions_list;

ArchitectureInterface * getCompArch(CompilerArch target_arch);
ArchitectureInterface * getJITArch(CompilerArch target_arch);




jit_code_t * create_JITCode(size_t memory_size);
void JIT_append(jit_code_t*jit, const char *code, size_t cs);

void JIT_reaplace(jit_code_t*jit, const char *code, size_t code_size,size_t pos);


#endif 