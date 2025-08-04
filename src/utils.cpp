#include "utils.hpp"

CompilerArch system_arch;

CompilerArch getTargetArch(const std::string &arch) {
  if (arch == "x86") {
    return CompilerArch::X86_A;
  } else if (arch == "x86_64") {
    return CompilerArch::X86_64_A;
  } else if (arch == "arm32") {
    return CompilerArch::ARM32_A;
  } else if (arch == "arm64") {
    return CompilerArch::ARM64_A;
  } else if (arch == "riscv") {
    return CompilerArch::RISCV_A;
  } else if (arch == "dlx") {
    return CompilerArch::DLX_A;
  } else {
    std::cerr << "Unknown target architecture: " << arch << std::endl;
    exit(EXIT_FAILURE);
  }
}

void getSystemArch(){
  struct utsname un;
  if (uname(&un) == -1) {
    perror("uname");
    return;
  }
  
  system_arch = getTargetArch(un.machine); // Get the machine hardware name
}

FILE * fileWrite(const char *filename) {
  FILE *file = fopen(filename, "w+");
  if (!file) {
    std::cerr << "Error creating file: " << filename << std::endl;
    exit(EXIT_FAILURE);
  }
  return file;
}

FILE * fileRead(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    std::cerr << "Error opening file: " << filename << std::endl;
    exit(EXIT_FAILURE);
  }
  return file;
}

void verbose(CompilerOptions options, const std::string &message) {
  if (options.verbose) {
    std::cout << message << std::endl;
  }
}

ArchitectureInterface * getCompArch(CompilerArch target_arch){
  switch (target_arch) {
    case CompilerArch::X86_A:
      return new X86();
    case CompilerArch::X86_64_A:
      return new X86();
    case CompilerArch::ARM32_A:
      return new ARM32();
  }
  return NULL;
}

JITInterface * getJITArch(CompilerArch target_arch){
  // switch (target_arch) {
  //   case CompilerArch::X86_A:
  //     return new X86JIT();
  //   case CompilerArch::X86_64_A:
  //     return new X86JIT();
    
  // }
  return NULL;
}

CompilerOptions getCompilerOptions(int argc, char* argv[]) {
  CompilerOptions options;
  options.optimize = true; // Default optimization flag
  options.debug = false; // Default debugging flag
  options.verbose = false; // Default verbose output flag

  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <source_file.bf>" << std::endl;
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    
    if(arg == "--optimize" || arg == "-O") {
      options.optimize = false;
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

void hexDump(jit_code_t* jit) {
  std::cout << "Generated machine code (" << jit->code_size << " bytes):" << std::endl;
  for (size_t i = 0; i < jit->code_size; ++i) {
    printf("%02X ", ((unsigned char*)jit->code_buf)[i]);
    if(i % 16 == 15) {
        std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}

