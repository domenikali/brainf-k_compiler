#include <iostream>
#include <sys/utsname.h>
#include <string>

std::string system_arch;

void get_system_arch(){
  struct utsname un;
  if (uname(&un) == -1) {
    perror("uname");
    return;
  }
  
  system_arch = un.machine; // Get the machine hardware name
}


int main(int argc, char* argv[]) {
  if(argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <source_file.bf>" << std::endl;
    return 1;
  }

  std::string source_file_name = argv[1];
  if(source_file_name.find_last_of('.')==std::string::npos || source_file_name.substr(source_file_name.find_last_of('.')) != ".bf") {
    std::cerr << "Error: Source file must have a .bf extension." << std::endl;
      return 1;
  }

  std::string output_file_name = source_file_name.substr(0, source_file_name.find_last_of('.')) + ".asm";
  std::cout << "Compiling Brainfuck source file: " << source_file_name << " as: "<<output_file_name<< std::endl;
  
  get_system_arch();
  std::cout << "Detected system architecture: " << system_arch << std::endl;

  return 0;
}