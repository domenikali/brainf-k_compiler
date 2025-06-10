#ifndef ARM32_H
#define ARM32_H
#include "../architecture_interface.hpp"
class ARM32: public ArchitectureInterface {
  public:
    ARM32(){
      std::cout<<"arm32 architecture"<<std::endl;
    };

    std::string proStart(uint64_t tape_size)override{
      return ".bss\n.lcomm tape,"+std::to_string(tape_size)+"\n.text\n.global _start \n_start:\nldr r1, =tape\nmov r2,#1\n";
    };
    
    virtual std::string proEnd()override{
      return "mov r0, #0\nmov r7, #1\nswi 0";
    };
    
    virtual std::string add(uint8_t count)override{
      return "ldrb r0, [r1]\nadd r0,r0,#"+std::to_string(count)+"\nstrb r0, [r1]\n";
    };
    
    virtual std::string sub(uint8_t count)override{
      return "ldrb r0, [r1]\nsub r0,r0,#"+std::to_string(count)+"\nstrb r0, [r1]\n";
    };
    
    virtual std::string output(uint8_t count)override{
      return "mov r7,#4\nmov r0,#1\nswi 0\n";
    };
    
    virtual std::string input(uint8_t count)override{
      return "mov r7,#3\nmov r0,#1\nswi 0\n";
    };
    
    virtual std::string inc(uint8_t count)override{
      return "add r1, r1, #"+std::to_string(count)+"\n";
    };
    
    virtual std::string dec(uint8_t count)override{
      return "sub r1, r1, #"+std::to_string(count)+"\n";
    };
    
    virtual std::string bneq(uint64_t pc, uint64_t jump)override{
      return "lable_"+std::to_string(pc)+":\n ldrb r0, [r1]\ncmp r0, #0\nbne lable_"+std::to_string(jump)+"\n";
    };
    
    virtual std::string beqz(uint64_t pc, uint64_t jump)override{
      return "ldrb r0, [r1]\ncmp r0, #0\nbeq lable_"+std::to_string(jump)+"\nlable_"+std::to_string(pc)+":\n ";
    };
          
};

#endif