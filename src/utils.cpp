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

jit_code_t* create_JITCode(size_t memory_size){
  jit_code_t*jit = (jit_code_t*)malloc(sizeof(jit_code_t));
  jit->code_buf = (unsigned char *)malloc(memory_size); // Allocate 1MB for code buffer
  if (!jit->code_buf) {
    std::cerr << "Error allocating memory for JIT code buffer." << std::endl;
    exit(EXIT_FAILURE);
  }
  jit->code_size = 0;
  jit->memory_size = memory_size;
  return jit;
}


void JIT_append(jit_code_t*jit,const char * code, size_t cs){
  if (jit->code_size + cs >= jit->memory_size) {
    std::cerr << "Error: JIT code buffer overflow." << std::endl;
    exit(EXIT_FAILURE);
  }
  
  memcpy(jit->code_buf + jit->code_size, code, cs);
  jit->code_size += cs;
}

void JIT_reaplace(jit_code_t*jit, const char *code, size_t code_size,size_t pos){
  for(int i=0;i<code_size;i++){
    if (pos + i >= jit->memory_size) {
      std::cerr << "Error: JIT code buffer overflow during replacement." << std::endl;
      exit(EXIT_FAILURE);
    }
    jit->code_buf[pos + i] = code[i];
  }
}

