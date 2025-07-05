#ifndef X86JIT_H
#define X86JIT_H
#include "../architecture_interface.hpp"
#include <cstring>

class X86JIT {
  public:
    X86JIT() {
      //std::cout << "x86 architecture" << std::endl;
    }
    const char * proStart(uint64_t tape_size){
      static char c[9]= "\x48\x89\xFE"                       //mov rsi,rdi; using rsi as tape pointer (rdi is the first argument in x86-64 syscall convention)
              "\xBF\x01\x00\x00\x00";             // mov edi, 1 (stdout); preloading the syscall number for write
      return c;
      };
    
    const char * proEnd(){
      return "\xB8\x3C\x00\x00\x00"               // mov eax, 60; (sys_exit)
             "\xBf\x00\x00\x00\x00"               // xor edi, edi; (exit code 0)
             "\x0F\x05";                          // syscall
    };
    
    const char * add(uint8_t count){
      static char c[10]= "\x48\x8B\x06"         // mov rax, [rsi]; load current cell value into rax
                  "\x48\x83\xC0";        // add rax, count; add the count to rax
      c[6]=static_cast<char>(count & 0xFF);
      memcpy(c+7,"\x48\x89\x06",3);               // mov [rsi], rax; store back to tape
      return c;  
    };
    
      const  char * sub(uint8_t count){
      static char c[10]= "\x48\x8B\x06"         // mov rax, [rsi]; load current cell value into rax
                           "\x48\x83\xE8";        // sub rax, count; sub the count to rax    
      c[6]=static_cast<char>(count & 0xFF);        
      memcpy(c+7,"\x48\x89\x06",3);               // mov [rsi], rax; store back to tape
      return c;
    };
    
    const  char * output(uint8_t count){
      return "\xB8\x01\x00\x00\x00"               // mov eax, 1; (sys_write)  
             "\xBA\x01\x00\x00\x00"               // mov edx, 1; number of bytes to write
             "\x0F\x05";                          // syscall; syscall
    };
    
    const  char * input(uint8_t count){
      return "\xB8\x00\x00\x00\x00"               // mov eax, 1; (sys_read)  
             "\xBA\x01\x00\x00\x00"               // mov edx, 1; number of bytes to write
             "\x0F\x05"; 
    };
    
    const  char * inc(uint8_t count){
      static char c[4] = "\x48\x83\xC6";        // add rsi, count; increment tape pointer
      c[3] = static_cast<char>(count & 0xFF);
      return c;         
    };
    
    const  char * dec(uint8_t count){
      static char c[4] = "\x48\x83\xEE";        // sub rsi, count; decrement tape pointer
      c[3] = static_cast<char>(count & 0xFF);
      return c;

    };
    
    const char * bneq(uint64_t pc, uint64_t jump){

      int32_t offset = static_cast<int32_t>((jump) - (pc+13)); // Calculate the offset for the jump
      static char c[13] = "\x48\x8B\x06"                       // mov rax, [rsi]; load current cell value into rax    
                    "\x48\x83\xF8\x00"                   // cmp rax, 0; compare rax with 0 
                    "\x0F\x85" ;                          // jne lable_jump; jump if not equal
      c[9] = static_cast< char>(offset & 0xFF);         // Set the low byte of the offset
      c[10] = static_cast< char>((offset >> 8) & 0xFF); // Set the second byte of the offset
      c[11] = static_cast< char>((offset >> 16) & 0xFF); // Set the third byte of the offset
      c[12] = static_cast< char>((offset >> 24) & 0xFF); // Set the high byte of the offset
      return c;

      
      //return "lable_"+std::to_string(pc)+":\n mov rax, [rsi]\ncmp rax, 0\n jne lable_"+std::to_string(jump)+"\n";
    };
    
    const  char * beqz(){
      return "\x48\x8B\x06"                       // mov rax, [rsi]; load current cell value into rax    
             "\x48\x83\xF8\x00"                   // cmp rax, 0; compare rax with 0 
             "\x0F\x84\x00\x00\x00\x00";          // je adress computed in the jit compiler
      //return "mov rax, [rsi]\ncmp rax, 0\nje lable_"+std::to_string(jump)+"\nlable_"+std::to_string(pc)+":\n ";
    };
};

#endif