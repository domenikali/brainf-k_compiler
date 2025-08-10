#include "debugger.hpp"

FILE* debug_file(std::string file_name){
  file_name = file_name.substr(0, file_name.find_last_of('.')) + ".dbg"; // Ensure the file has .dbg extension
  if (file_name.find_last_of('.') == std::string::npos) {
    file_name += ".dbg"; // Append .dbg if no extension is present
  }
  FILE* file = fopen(file_name.c_str(), "w");
  if (!file) {
    std::cerr << "Error opening debug file: " << file_name << std::endl;
    exit(EXIT_FAILURE);
  }
  return file;
}

void debug(instructions_list instructions, CompilerOptions options) {

  FILE* debug_file_name = debug_file(options.source_file_name);
  verbose(options, "Debugging enabled. Debug file created: " + options.source_file_name);
  std::cout << "Debugging information: No .asm produced" << std::endl;
  
  fprintf(debug_file_name, "Debugging information:\nDebug file: %s\n", options.source_file_name.c_str());
  char ch;
  uint64_t head = 0;
  uint64_t size = options.max_memory > 100 ? 100 : options.max_memory; // Default size for Brainfuck memory
  std::vector<uint8_t> memory(size, 0); // Initialize memory with zeros

  std::string total_output="";
  std::stack<uint64_t> cycle_stack; // Stack to manage cycles
  uint64_t pc = 0;
  
  while(pc<instructions.size()) {
    Instruction instruction = instructions[pc];
    switch (instruction.type){
    case InstructionType::ADD:
      fprintf(debug_file_name, "[PC %ld]: Increased value at %ld from %u to %u.\n",pc, head, memory[head], memory[head]+1);
      memory[head]++;
      break;
    case InstructionType::SUB:
      fprintf(debug_file_name, "[PC %ld]: Decreased value at %ld from %u to %u.\n",pc, head, memory[head], memory[head]-1);
      memory[head]--;
      break;
    case InstructionType::OUTPUT:
      fprintf(debug_file_name, "[PC %ld]: Printing: value at %ld = %c.\n",pc, head, memory[head]);
      total_output += static_cast<char>(memory[head]);
      break;
    case InstructionType::INPUT:
      char input_char;
      std::cout << "[PC " <<pc<<"]:Enter a character for input: ";
      std::cin >> input_char;
      memory[head] = static_cast<uint8_t>(input_char);
      fprintf(debug_file_name, "[PC %ld]: Input char %c at %ld\n", pc, input_char, head);
      break;
    case InstructionType::INC:
      if(head + 1 >= options.max_memory) {
        fprintf(debug_file_name, "[PC %ld]: Pointer overflow at %ld. Max memory: %ld.\n", pc, head, options.max_memory);
        std::cerr << "Pointer overflow at " << head << ". Max memory: " << options.max_memory << ". Exit Error." << std::endl;
        goto ext;
      } else if(head + 1 >= size) {
        uint64_t inc = options.max_memory ? options.max_memory : size;
        memory.resize(size + inc, 0); 
        size += inc;
      }

      fprintf(debug_file_name, "[PC %ld]: Pointer increased from %ld to %ld.\n",pc, head, head+1);
      head++;
      break;
    case InstructionType::DEC:
      if(head == 0) {
        fprintf(debug_file_name, "[PC %ld]: Pointer underflow at %ld.\n", pc, head);
        std::cerr << "Pointer underflow at " << head << ". Exit Error." << std::endl;
        goto ext;
      }

      fprintf(debug_file_name, "[PC %ld]: Pointer decremented from %ld to %ld.\n",pc, head, head-1);
      head--;
      break;
    case InstructionType::BEQZ:
      if(memory[head] == 0) {
        fprintf(debug_file_name, "[PC %ld]: Pointer at %ld is zero, jumping to %d.\n", pc, head, instruction.extra);
        pc = instruction.extra; // Jump to the branch address
      } else {
        fprintf(debug_file_name, "[PC %ld]: Pointer at %ld is non-zero, continuing.\n", pc, head);
      }
      break;
    case InstructionType::BNEQ:
      if(memory[head] != 0) {
        fprintf(debug_file_name, "[PC %ld]: Pointer at %ld is non-zero, jumping to %d.\n", pc, head, instruction.extra);
        pc = instruction.extra; // Jump to the branch address
      } else {
        fprintf(debug_file_name, "[PC %ld]: Pointer at %ld is zero, continuing.\n", pc, head);
      }
      break;
    
    default:
      break;
    }
    pc++;
  }
  ext: 
  fprintf(debug_file_name, "Total output: %s\n", total_output.c_str());
  fclose(debug_file_name);
  exit(EXIT_SUCCESS);

}

