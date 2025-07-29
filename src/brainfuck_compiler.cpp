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


#define INT32_S 4


CompilerOptions getCompilerOptions(int argc, char* argv[]) {
  CompilerOptions options;

  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <source_file.bf>" << std::endl;
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    
    if(arg == "--optimize" || arg == "-O") {
      options.optimize = true;
    } else if(arg == "--debug" || arg == "-D") {
      options.debug = true;
    } else if(arg == "--jit" || arg == "-J") {
      options.jit = true;
    } else if(arg == "--verbose" || arg == "-V") {
      options.verbose = true;
    } else if(arg == "--max-cycles" || arg == "-C") {
      if (i + 1 < argc) {
        options.max_cycles = std::stoull(argv[++i]);
      } else {
        std::cerr << "Error: --max-cycles requires a value." << std::endl;
        exit(EXIT_FAILURE);
      }
    } else if(arg == "--max-memory" || arg == "-M") {
      if (i + 1 < argc) {
        options.max_memory = std::stoull(argv[++i]);
      } else {
        std::cerr << "Error: --max-memory requires a value." << std::endl;
        exit(EXIT_FAILURE);
      }
    } else if(arg == "--target-arch" || arg == "-T") {
      if (i + 1 < argc) {
        options.target_arch = getTargetArch(argv[++i]);
      } else {
        std::cerr << "Error: --target-arch requires a value." << std::endl;
        exit(EXIT_FAILURE);
      }
    }else if(arg == "--name"|| arg == "-N") {
      if (i + 1 < argc) {
        std::string file_name = argv[++i];
        if(file_name.find_last_of('.') != std::string::npos) {
          file_name = file_name.substr(0, file_name.find_last_of('.'))+ ".asm"; // Ensure the file has .asm extension
        }
        else{
          file_name += ".asm"; // Append .asm if no extension is present
        }
        options.output_file_name = file_name;
      } else {
        std::cerr << "Error: --name requires a value." << std::endl;
        exit(EXIT_FAILURE);
      }
    } else if(arg == "--help" || arg == "-h") {
      std::cout << "Usage: " << argv[0] << " [options] <source_file.bf>" << std::endl;
      std::cout << "Options:" << std::endl;
      std::cout << "\t-O, --optimize          Disable optimizations" << std::endl;
      std::cout << "\t-J, --jit               Enable Just In Time Compiler" << std::endl;
      std::cout << "\t-D, --debug             Stop compilation and create a debug file with extended informations about the program" << std::endl;
      std::cout << "\t-V, --verbose           Enable verbose output" << std::endl;
      std::cout << "\t-C, --max-cycles <n>    Set maximum cycles to <n>, default 1000000" << std::endl;
      std::cout << "\t-M, --max-memory <n>    Set maximum memory to <n>, default 3000" << std::endl;
      std::cout << "\t-T, --target-arch <arch>Set target architecture, default detect sys arch" << std::endl;
      std::cout << "\t-N, --name <name>       Set output file name, default source file" << std::endl;
      std::cout << "\t-h, --help              Show this help message" << std::endl;
      exit(0);
    }else if(arg.find("--") == 0 || arg.find("-") == 0) {
      std::cerr << "Error: Unknown option '" << arg << "'." << std::endl;
      exit(EXIT_FAILURE);
    }else {
      if(arg.find_last_of('.')==std::string::npos || arg.substr(arg.find_last_of('.')) != ".bf") {
        std::cerr << "Error: Source file must have a .bf extension." << std::endl;
          exit(EXIT_FAILURE);
      }
      options.source_file_name = arg;
    }
  }

  if(options.output_file_name.empty()) {
    options.output_file_name = options.source_file_name.substr(0, options.source_file_name.find_last_of('.')) + ".asm";
  }
  if(options.target_arch==CompilerArch::UNKNOWN) {
    getSystemArch();
    options.target_arch = system_arch; // Default detected system architecture
  }
  if(options.max_cycles == 0) {
    options.max_cycles = 1000000; // Default maximum cycles, i kind of not use this option but maybe?
  }
  if(options.max_memory == 0) {
    options.max_memory = 30000; // Default maximum memory size
  }
  return options;
}

std::vector<Instruction> lexer(CompilerOptions options,std::map<InstructionType,uint16_t> &instructions_map) { 
  FILE * source_file = fileRead(options.source_file_name.c_str());
  std::vector<Instruction> instructions;
  char ch;
  uint64_t pc = 0;

  if(options.debug)
    options.optimize = false; 

  std::stack<uint64_t> cycle_stack;
  while((ch=fgetc(source_file)) != EOF) {
    Instruction instruction;
    instruction.extra = 1; 
    instruction.type = InstructionType::UNKNOWN; 

    switch (ch)
    {
    case '+':
      while((ch=fgetc(source_file)) == '+'&&options.optimize) {
        instruction.extra++;
      }
      if(ch != EOF) fseek(source_file, -1, SEEK_CUR); 
      instruction.type = InstructionType::ADD;
      instructions.push_back(instruction);
      
      break;
    case '-':
      while((ch=fgetc(source_file)) == '-'&&options.optimize) {
        instruction.extra++;
      }
      if(ch != EOF) fseek(source_file, -1, SEEK_CUR); 
      instruction.type = InstructionType::SUB;
      instructions.push_back(instruction);
      break;
    case '.':
      instruction.type = InstructionType::OUTPUT;
      instructions.push_back(instruction);
      break;
    case ',':
      instruction.type = InstructionType::INPUT;
      instructions.push_back(instruction);
      break;
    case '>':
      while((ch=fgetc(source_file)) == '>'&&options.optimize) {
        instruction.extra++;
      }
      if(ch != EOF) fseek(source_file, -1, SEEK_CUR); 
      instruction.type = InstructionType::INC;
      instructions.push_back(instruction);
      break;
    case '<':
      while((ch=fgetc(source_file)) == '<'&&options.optimize) {
        instruction.extra++;
      }
      if(ch != EOF) fseek(source_file, -1, SEEK_CUR); 
      instruction.type = InstructionType::DEC;
      instructions.push_back(instruction);
      break;
    case '[':
      instruction.type = InstructionType::BEQZ;
      cycle_stack.push(pc); 
      instructions.push_back(instruction);
      break;
    case ']':
      if(cycle_stack.empty()) {
        std::cerr << "Error: Unmatched ']' at program counter " << pc << std::endl;
        fclose(source_file);
        exit(EXIT_FAILURE);
      }
      instruction.type = InstructionType::BNEQ;
      instruction.extra = cycle_stack.top();
      cycle_stack.pop();
      instructions[instruction.extra].extra = pc; 
      instructions.push_back(instruction);
      break;
    default:
      break;
    }
    if(instruction.type != InstructionType::UNKNOWN) {
      instructions_map[instruction.type] += 1; 
      pc++;
    }
  }
  if(!cycle_stack.empty()){
    std::cerr << "Error: Unmatched '[' at program counter " << cycle_stack.top() << std::endl;
    fclose(source_file);
    exit(EXIT_FAILURE);
  }
  fclose(source_file);
  return instructions;
}

void hexDump(jit_code_t* jit) {
  std::cout << "Generated machine code (" << jit->code_size << " bytes):" << std::endl;
  for (size_t i = 0; i < jit->code_size; ++i) {
    //printf("%02X ", (unsigned char)jit->code_buf[i]);
    if(i % 16 == 15) {
        std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}

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
        program += arch->input(instruction.extra);
      break;
      case InstructionType::OUTPUT:
        program += arch->output(instruction.extra);
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
      case InstructionType::BEQZ:

        arch->beqz(jit);
        instructions[instruction.extra].extra = jit->code_size; // Store the address of the bneq instruction
      break;
      case InstructionType::BNEQ:

        arch->bneq(jit,instruction.extra);

        int32_t jump_distance = static_cast<int32_t>(jit->code_size - instruction.extra);
        
        memcpy(jit->code_buf + instruction.extra-branch_adress_size, &jump_distance, INT32_S); // Patch the jump distance
      break; 
    }
    pc++;
  }
  arch->proEnd(jit);
  
  
  verbose(options, "Compilation completed successfully. Preparing memory for JIT execution.");
  
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
  
  run(mem);

  munmap(jit->code_buf, jitSize);
  free(mem);
  delete arch;
}


int main(int argc, char* argv[]) {

  CompilerOptions options = getCompilerOptions(argc, argv);  
  
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
  instructions_list instructions = lexer(options,instructions_map);
  verbose(options, "Translation completed");
  if(options.debug) {
    std::cout << "Debugging enabled." << std::endl;
    debug(instructions, options);
  }
  if(options.jit) {
    verbose(options, "Just-In-Time compilation enabled.");
    jit_compiler(instructions, options,instructions_map);
  }
  else{
    verbose(options, "Compiling..."); 
    compiler(instructions,options);

  }


  return 0;
}