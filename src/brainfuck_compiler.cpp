#include <iostream>
#include <sys/utsname.h>
#include <string>
#include <cstdint>
#include "debugger.cpp"
#include "utils.hpp"

Compiler_Options get_compiler_options(int argc, char* argv[]) {
  Compiler_Options options;

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
        options.target_arch = argv[++i];
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
      std::cout << "\t-D, --debug             Stop compilation and create a debug file with extended informations about the program" << std::endl;
      std::cout << "\t-V, --verbose           Enable verbose output" << std::endl;
      std::cout << "\t-C, --max-cycles <n>   Set maximum cycles to <n>, default 1000000" << std::endl;
      std::cout << "\t-M, --max-memory <n>   Set maximum memory to <n>, default 3000" << std::endl;
      std::cout << "\t-T, --target-arch <arch> Set target architecture, default detect sys arch" << std::endl;
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
  if(options.target_arch.empty()) {
    get_system_arch();
    options.target_arch = system_arch; // Default detected system architecture
  }
  if(options.max_cycles == 0) {
    options.max_cycles = 1000000; // Default maximum cycles
  }
  if(options.max_memory == 0) {
    options.max_memory = 30000; // Default maximum memory size
  }
  return options;
}


int main(int argc, char* argv[]) {
  

  Compiler_Options options = get_compiler_options(argc, argv);  

  std::cout << "Compiling Brainfuck source file: " << options.source_file_name << " as: "<<options.output_file_name<< std::endl;
  
  std::cout << "Architecture output: " << options.target_arch << std::endl;

  if(options.debug) {
    std::cout << "Debugging enabled." << std::endl;
    debug(argc,argv, options);
  }



  return 0;
}