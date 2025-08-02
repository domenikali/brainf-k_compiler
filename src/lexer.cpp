#include "lexer.hpp"
std::vector<Instruction> lexer(CompilerOptions options,std::map<InstructionType,uint16_t> &instructions_map) { 
    FILE* file = fopen(options.source_file_name.c_str(), "rb");
    if (!file) {
      std::cerr << "Error: Could not open source file '" << options.source_file_name << "'." << std::endl;
      exit(EXIT_FAILURE);
    }
    
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(size);
    if (!buffer) {
      fclose(file);
      std::cerr << "Error: Lexer memory allocation failed." << std::endl;
      exit(EXIT_FAILURE);
    }
    
    // Read entire file
    size_t read = fread(buffer, 1, size, file);
    fclose(file);
    
    if (read != size) {
      free(buffer);
      std::cerr << "Error: Could not read the entire source file." << std::endl;
      exit(EXIT_FAILURE);
    }
  
    std::vector<Instruction> instructions;
    uint64_t pc = 0;
  
    if(options.debug)
      options.optimize = false; 
    
    std::stack<uint64_t> cycle_stack;
    int i=0;
    while(i<size) {
      Instruction instruction;
      instruction.extra = 1; 
      instruction.type = InstructionType::UNKNOWN; 
  
      switch (buffer[i])
      {
      case '+':
        while(buffer[i+1] == '+' && options.optimize) {
          instruction.extra++;
          i++;
        }
        instruction.type = InstructionType::ADD;
        instructions.push_back(instruction);
        
        break;
      case '-':
  
        while(buffer[i+1] == '-'&&options.optimize) {
          instruction.extra++;
          i++;
        }
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
  
        while(buffer[i+1] == '>'&&options.optimize) {
          instruction.extra++;
          i++;
        }
        instruction.type = InstructionType::INC;
        instructions.push_back(instruction);
        break;
      case '<':
        while(buffer[i+1] == '<'&&options.optimize) {
          instruction.extra++;
          i++;
        }
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
      i++;
    }
    if(!cycle_stack.empty()){
      std::cerr << "Error: Unmatched '[' at program counter " << cycle_stack.top() << std::endl;
      exit(EXIT_FAILURE);
    }
    free(buffer);
    return instructions;
  }
  