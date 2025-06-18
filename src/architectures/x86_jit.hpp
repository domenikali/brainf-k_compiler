#ifndef X86JIT_H
#define X86JIT_H
#include "../architecture_interface.hpp"
class X86JIT: public ArchitectureInterface {
  public:
    X86JIT() {
      std::cout << "x86 architecture" << std::endl;
    }
    std::string proStart(uint64_t tape_size)override{
      return "section\t.bss\ntape: resd "+std::to_string(tape_size)+"\nsection .text\nglobal _start \n_start:\nmov rsi, tape\nmov rdi, 1\n";
    };
    
    virtual std::string proEnd()override{
      return "mov rax, 10\nmov [rsi], rax\n"+this->output(1)+"mov rax, 60\nmov rdi, 0\nsyscall\n";
    };
    
    virtual std::string add(uint8_t count)override{
      return "mov rax, [rsi]\nadd rax, "+std::to_string(count)+"\nmov [rsi], rax\n";
    };
    
    virtual std::string sub(uint8_t count)override{
      return "mov rax, [rsi]\nsub rax, "+std::to_string(count)+"\nmov [rsi], rax\n";
    };
    
    virtual std::string output(uint8_t count)override{
      return "mov rax, 1\nmov rdx, "+std::to_string(count)+"\nsyscall\n";
    };
    
    virtual std::string input(uint8_t count)override{
      return "mov rax, 0\nmov rdx, "+std::to_string(count)+"\nsyscall\n";
    };
    
    virtual std::string inc(uint8_t count)override{
      return "add rsi, "+std::to_string(count)+"\n";
    };
    
    virtual std::string dec(uint8_t count)override{
      return "sub rsi, "+std::to_string(count)+"\n";
    };
    
    virtual std::string bneq(uint64_t pc, uint64_t jump)override{
      return "lable_"+std::to_string(pc)+":\n mov rax, [rsi]\ncmp rax, 0\n jne lable_"+std::to_string(jump)+"\n";
    };
    
    virtual std::string beqz(uint64_t pc, uint64_t jump)override{
      return "mov rax, [rsi]\ncmp rax, 0\nje lable_"+std::to_string(jump)+"\nlable_"+std::to_string(pc)+":\n ";
    };


};

#endif