#include <iostream>
#include <sys/utsname.h>
#include <string>
#include <cstdint>
#include "debugger.hpp"
#include "architecture_interface.hpp"
#include <vector>
#include "comp_arch/arm32.hpp"
#include "comp_arch/x86.hpp"
#include "JIT_arch_iterface.hpp"
#include <sys/mman.h>
#include "jit_arch/x86_jit.hpp"
#include <stack>
#include <chrono>
#include <fstream>
#include <streambuf>
#include "lexer.hpp"

#define OPT_MOV0 2 //the size of the move 0 instruction [+] or [-]
#define OPT_ADDTO 5 // the size of the add to instruction [-<+>] || [->+<]
#define INT32_S 4

void compiler(instructions_list instructions,CompilerOptions options){
  ArchitectureInterface *arch = getCompArch(options.target_arch);
  verbose(options, "Target architecture: " );

  uint64_t pc =0;
  std::string program ="";
  program+= arch->proStart(options.max_memory);
  for(Instruction instruction : instructions){
    switch(instruction.type){
      case InstructionType::ADD:
        program += arch->add(instruction.extra);
      break;
      case InstructionType::SUB:
        program += arch->sub(instruction.extra);
      break;
      case InstructionType::INC:
        program += arch->inc(instruction.extra);
      break;
      case InstructionType::DEC:
        program += arch->dec(instruction.extra);
      break;
      case InstructionType::INPUT:
        program += arch->input();
      break;
      case InstructionType::OUTPUT:
        program += arch->output();
      break;
      case InstructionType::BEQZ:
        program += arch->beqz(pc,instruction.extra);
      break;
      case InstructionType::BNEQ:
        program += arch->bneq(pc,instruction.extra);
      break; 
    }
    pc++;
  }
  program += arch->proEnd();
  verbose(options, "Compilation completed successfully.");
  std::cout << "Output written to: " << options.output_file_name << std::endl;
  std::cout << "Program size: " << instructions.size() << " instructions." << std::endl;
  delete arch; // Clean up architecture object
  FILE * outputFile = fileWrite(options.output_file_name.c_str());
  fprintf(outputFile, "%s", program.c_str());
  fclose(outputFile);
  verbose(options, "Output file written successfully.");
  verbose(options,program.c_str());

}

void jit_compiler(instructions_list instructions,CompilerOptions options,std::map<InstructionType,uint16_t> &instructions_map) {
  //ArchitectureInterface *arch = getJITArch(options.target_arch); 
  // using std::chrono::duration_cast;
  // using std::chrono::nanoseconds;
  // typedef std::chrono::high_resolution_clock clock;

  //auto start = clock::now();
  uint8_t branch_adress_size;
  JIT_init_t init;

  JITInterface *arch = new X86JIT(&init); 

  size_t jitSize=0;
  for(auto &pair : instructions_map) {
    
    jitSize += pair.second * init.instructions_size[static_cast<uint8_t>(pair.first)]; 
  }
  jitSize+= init.instructions_size[static_cast<uint8_t>(InstructionType::UNKNOWN)]; // Add size for proStart and proEnd
  verbose(options, "JIT code size: " + std::to_string(jitSize) + " bytes.");
  branch_adress_size = init.branch_address_size;

  uint64_t pc =0;
  jit_code_t*jit = create_JITCode(jitSize);
  std::stack<uint32_t> branch_stack; // Stack to handle branches
  arch->proStart(jit);
  for(Instruction instruction : instructions){
    switch(instruction.type){
      case InstructionType::ADD:
        arch->add(jit,instruction.extra);
      break;
      case InstructionType::SUB:
        arch->sub(jit,instruction.extra);
        break;
      case InstructionType::INC:
        arch->inc(jit,instruction.extra);
      break;
      case InstructionType::DEC:
        arch->dec(jit,instruction.extra);
      break;
      case InstructionType::INPUT:
        arch->input(jit);
      break;
      case InstructionType::OUTPUT:
        arch->output(jit);
      break;
      case InstructionType::MOV0:
        arch->mov0(jit);
      break;
      case InstructionType::ADDTO:
        arch->addto(jit,instruction.extra);
      break;
      case InstructionType::BEQZ:

        arch->beqz(jit);
        branch_stack.push(jit->code_size);
      break;
      case InstructionType::BNEQ:
        uint32_t branch_address = branch_stack.top();
        branch_stack.pop();
        arch->bneq(jit,branch_address);

        int32_t jump_distance = static_cast<int32_t>(jit->code_size - branch_address);
        
        memcpy((char*)jit->code_buf + branch_address-branch_adress_size, &jump_distance, INT32_S); // Patch the jump distance
      break; 
    }
    pc++;
  }
  arch->proEnd(jit);

  //auto end = clock::now();
  //std::cout << "JIT compilation completed in: " << duration_cast<nanoseconds>(end-start).count() << "ns" << std::endl;
  
  
  verbose(options, "Compilation completed successfully. Preparing memory for JIT execution.");
  //hexDump(jit);
  void *mem = calloc(options.max_memory, sizeof(uint8_t));
  if (mem == NULL) {
    std::cerr << "Error: Memory allocation failed." << std::endl;
    delete arch;
    exit(EXIT_FAILURE);
  }
  verbose(options, "Memory allocated successfully.");


  if (mprotect(jit->code_buf, jit->memory_size, PROT_READ | PROT_EXEC) != 0) {
    std::cerr << "Error: Failed to make memory executable." << std::endl;
    munmap(jit->code_buf, jit->memory_size);
    free(mem);
    delete arch;
    exit(EXIT_FAILURE);
  }
  verbose(options, "Memory made executable successfully.");

  // Execute the JIT compiled code
  void (*run)(void *memory) = (void (*)(void*))jit->code_buf;
  //start = clock::now();
  run(mem);
  //end = clock::now();
  //std::cout << "JIT execution completed in: " << duration_cast<nanoseconds>(end-start).count() << "ns" << std::endl;
  verbose(options, "JIT execution completed successfully.");
  //munmap(jit->code_buf, jitSize);
  //free(mem);
  delete arch;
}


/**
 * main optimisation passes:
 * -  [-] || [+] -> move_0
 * -  []
 */
void compilerPasses(instructions_list &instructions,CompilerOptions options) {
  verbose(options, "Starting compiler passes for optimization.");
  std::stack<uint32_t> branch_stack;
  for(size_t j=0;j<instructions.size();j++){
    Instruction i = instructions[j];
    if(i.type==InstructionType::BEQZ){
      branch_stack.push(j);
    }
    if(i.type==InstructionType::BNEQ){
      
      uint32_t branch_address = branch_stack.top();
      branch_stack.pop();
      int cas =j-branch_address;
      
      switch(cas){
        case OPT_MOV0:// move 0 instructions or find next free
          if(instructions[j-1].type==InstructionType::ADD || instructions[j-1].type==InstructionType::SUB){
            instructions[branch_address].type = InstructionType::MOV0;
            instructions.erase(instructions.begin()+j-1);
            instructions.erase(instructions.begin()+j-1);
            j-=2; // Adjust index after erasing instructions
          }
        break;
        case OPT_ADDTO:// add current cell to n-th then zero current cell
          if(((instructions[j-1].type==InstructionType::INC && instructions[j-3].type==InstructionType::DEC) || 
          (instructions[j-1].type==InstructionType::DEC && instructions[j-3].type==InstructionType::INC)) &&
          instructions[j-3].extra==instructions[j-1].extra&& instructions[j-2].type==InstructionType::ADD&&
          instructions[j-4].type==InstructionType::SUB){
            
            instructions[j].type = InstructionType::ADDTO;
            //to compute both [->-<] right and left [-<->] it's sufficent to change the signe of the operand
            if(instructions[j-1].type==InstructionType::INC)instructions[j].extra = static_cast<uint8_t>(-instructions[j-1].extra);
            else instructions[j].extra = static_cast<uint8_t>(instructions[j-1].extra);
            
            instructions.erase(instructions.begin()+j-5,instructions.begin()+j);
            j-=5; 
          }
        break;

      }
    }
  }
  // for(int k=0;k<instructions.size();k++){
  //   std::cout << "Instruction " << k << ": Type = " << static_cast<char>(instructions[k].type) 
  //             << ", Extra = " << static_cast<int>(instructions[k].extra )<< std::endl;
  // }

}



int main(int argc, char* argv[]){
  // using std::chrono::duration_cast;
  // using std::chrono::nanoseconds;
  // typedef std::chrono::high_resolution_clock clock;
  // auto start = clock::now();
  CompilerOptions options = getCompilerOptions(argc, argv);  
  //auto end = clock::now();
  //std::cout <<"compiler options: "<< duration_cast<nanoseconds>(end-start).count() << "ns"<<std::endl;

  verbose(options, "Compiling Brainfuck source file: "+options.source_file_name+" as: "+options.output_file_name);
  std::map<InstructionType,uint16_t> instructions_map= {
    {InstructionType::ADD, 0},
    {InstructionType::SUB, 0},
    {InstructionType::INC, 0},
    {InstructionType::DEC, 0},
    {InstructionType::INPUT, 0},
    {InstructionType::OUTPUT, 0},
    {InstructionType::BEQZ, 0},
    {InstructionType::BNEQ, 0},
    {InstructionType::UNKNOWN, 0}
  };

  //start = clock::now();
    
  instructions_list instructions = lexer(options,instructions_map);
  //end = clock::now();
  //std::cout <<"lexer: "<< duration_cast<nanoseconds>(end-start).count() << "ns"<<std::endl;
  verbose(options, "Translation completed");
  if(options.debug) {
    std::cout << "Debugging enabled." << std::endl;
    debug(instructions, options);
  }
  if(options.jit) {
    if(options.optimize){
      verbose(options, "Running compiler passes for optimization.");
      compilerPasses(instructions, options);
    }

    verbose(options, "Just-In-Time compilation enabled.");
    jit_compiler(instructions, options,instructions_map);
  }
  else{
    verbose(options, "Compiling..."); 
    compiler(instructions,options);

  }


  return 0;
}