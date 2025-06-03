#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <sys/utsname.h>
#include <cstdint>

std::string system_arch;

void get_system_arch(){
  struct utsname un;
  if (uname(&un) == -1) {
    perror("uname");
    return;
  }
  
  system_arch = un.machine; // Get the machine hardware name
}

struct Compiler_Options{
  std::string source_file_name="";
  std::string output_file_name="";
  bool optimize = true; // Optimization flag
  bool debug = false; // Debugging flag
  bool verbose = false; // Verbose output flag
  uint64_t max_cycles = 0; // Maximum cycles flag
  uint64_t max_memory = 0; // Maximum memory flag
  std::string target_arch = ""; // Default target architecture
};
typedef struct Compiler_Options Compiler_Options;



#endif 