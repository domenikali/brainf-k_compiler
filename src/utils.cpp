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
    case CompilerArch::ARM32_A:
      return new ARM32();
    case CompilerArch::ARM64_A:
  }
}