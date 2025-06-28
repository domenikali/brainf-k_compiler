#ifndef X86JIT_H
#define X86JIT_H
#include "../architecture_interface.hpp"
class X86JIT: public ArchitectureInterface {
  public:
    X86JIT() {
      std::cout << "x86 architecture" << std::endl;
    }
    std::string proStart(uint64_t tape_size)override{
      return "\x48\x89\xFE"                       //mov rsi,rdi; using rsi as tape pointer (rdi is the first argument in x86-64 syscall convention)
              "\xbf\x01\x00\x00\x00";             // mov edi, 1 (stdout); preloading the syscall number for write
    };
    
    virtual std::string proEnd()override{
      return "\xB8\x3C\x00\x00\x00"               // mov eax, 60; (sys_exit)
             "\xBf\x00\x00\x00\x00"               // xor edi, edi; (exit code 0)
             "\x0F\x05";                          // syscall
    };
    
    virtual std::string add(uint8_t count)override{
      return "\x48\x8B\x06"                       // mov rax, [rsi]; load current cell value into rax
             "\x48\x05" + toHex(count) +          // add rax, count; add the count to rax
             "\x48\x89\x06";                      // mov [rsi], rax; store back to tape
    };
    
    virtual std::string sub(uint8_t count)override{
      return "\x48\x8B\x06"                       // mov rax, [rsi]; load current cell value into rax
             "\x48\x2D" + toHex(count) +          // sub rax, count; sub the count to rax
             "\x48\x89\x06";                      // mov [rsi], rax; store back to tape
    };
    
    virtual std::string output(uint8_t count)override{
      return "\xB8\x01\x00\x00\x00"               // mov eax, 1; (sys_write)  
             "\xBA\x01\x00\x00\x00"               // mov edx, 1; number of bytes to write
             "\x0F\x05";                          // syscall; syscall
    };
    
    virtual std::string input(uint8_t count)override{
      return "\xB8\x00\x00\x00\x00"               // mov eax, 1; (sys_read)  
             "\xBA\x01\x00\x00\x00"               // mov edx, 1; number of bytes to write
             "\x0F\x05"; 
    };
    
    virtual std::string inc(uint8_t count)override{
      return "\x48\x81\xC6"+toHex(count);         // add rsi, count; increment tape pointer
    };
    
    virtual std::string dec(uint8_t count)override{
      return "\x48\x81\xEE"+toHex(count);         // sub rsi, count; decrement tape pointer
    };
    
    virtual std::string bneq(uint64_t pc, uint64_t jump)override{
      return "lable_"+std::to_string(pc)+":\n mov rax, [rsi]\ncmp rax, 0\n jne lable_"+std::to_string(jump)+"\n";
    };
    
    virtual std::string beqz(uint64_t pc, uint64_t jump)override{
      return "mov rax, [rsi]\ncmp rax, 0\nje lable_"+std::to_string(jump)+"\nlable_"+std::to_string(pc)+":\n ";
    };
};

#endif