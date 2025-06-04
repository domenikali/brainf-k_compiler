#include "utils.hpp"

CompilerArch system_arch;

CompilerArch getTargetArch(const std::string &arch) {
  if (arch == "x86") {
    return CompilerArch::X86;
  } else if (arch == "x86_64") {
    return CompilerArch::X86_64;
  } else if (arch == "arm32") {
    return CompilerArch::ARM32;
  } else if (arch == "arm64") {
    return CompilerArch::ARM64;
  } else if (arch == "riscv") {
    return CompilerArch::RISCV;
  } else if (arch == "dlx") {
    return CompilerArch::DLX;
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