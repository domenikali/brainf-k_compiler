#ifndef X86JIT_H
#define X86JIT_H
#include "../architecture_interface.hpp"
#include <cstring>

class X86JIT {
  public:
    X86JIT(uint8_t *branch_address_size) {
      //std::cout << "x86 architecture" << std::endl;
      *branch_address_size =4;
    }
    void proStart(jit_code_t *jit){
      check_size(jit, 8);
      memcpy(jit->code_buf + jit->code_size, 
        "\x48\x89\xFE"              // mov rsi,rdi; move memory pointer to rsi
        "\xBF\x01\x00\x00\x00", 8); // mov edi, 1; (stdout) for optimisation purposes
      jit->code_size += 8;
    };
    
    void proEnd(jit_code_t *jit){
      check_size(jit, 12);
      memcpy(jit->code_buf + jit->code_size, 
              "\xB8\x3C\x00\x00\x00"// mov eax, 60; (sys_exit)
              "\xBf\x00\x00\x00\x00"// xor edi, edi; (exit code 0)
              "\x0F\x05",12);// syscall           
      jit->code_size += 12;
    };
    
    void add(jit_code_t *jit,uint8_t count){
      check_size(jit, 9);
      memcpy(jit->code_buf + jit->code_size, 
              "\x8A\x06"// mov al, [rsi]; current cell value into rax
              "\x04", 3);  // add rax, count; add the count to rax
      memcpy(jit->code_buf+jit->code_size+3, &count, 1);; // hex value
      memcpy(jit->code_buf + jit->code_size + 4, 
              "\x88\x06", 3); // mov [rsi], rax; store back to tape
      jit->code_size += 6;

    };
    
    void sub(jit_code_t*jit,uint8_t count){
      check_size(jit, 6);
      memcpy(jit->code_buf + jit->code_size,
              "\x8A\x06"         // mov rax, [rsi]; load current cell value into rax
              "\x2C",3);        // sub rax, count; sub the count to rax    
      memcpy(jit->code_buf+jit->code_size+3, &count, 1);    //hex value    
      memcpy(jit->code_buf+jit->code_size+4,
              "\x88\x06",3);                    // mov [rsi], rax; store back to tape
      jit->code_size += 6;
    };
    
    void output(jit_code_t *jit){
      check_size(jit, 12);
      memcpy(jit->code_buf+jit->code_size, 
              "\xB8\x01\x00\x00\x00"               // mov eax, 1; (sys_write)  
              "\xBA\x01\x00\x00\x00"               // mov edx, 1; number of bytes to write
              "\x0F\x05",12);                      // syscall; syscall
      jit->code_size += 12;
    };
    
    void input(jit_code_t *jit){
      check_size(jit, 12);
      memcpy(jit->code_buf+jit->code_size,
             "\xB8\x00\x00\x00\x00"               // mov eax, 1; (sys_read)  
             "\xBA\x01\x00\x00\x00"               // mov edx, 1; number of bytes to write
             "\x0F\x05",12); 
      jit->code_size += 12;

    };
    
    void inc(jit_code_t *jit,uint8_t count){
      check_size(jit, 4);
      memcpy(jit->code_buf+jit->code_size,
              "\x48\x83\xC6",3);        // add rsi, count; increment tape pointer
      memcpy(jit->code_buf+jit->code_size+3, &count, 1); // Store the count value
      jit->code_size += 4;

    };
    
    void dec(jit_code_t *jit,uint8_t count){
      check_size(jit, 4);
      memcpy(jit->code_buf+jit->code_size,
              "\x48\x83\xEE",3);        // sub rsi, count; decrement tape pointer
      memcpy(jit->code_buf+jit->code_size+3, &count, 1);
      jit->code_size += 4;
  };
    
    void bneq(jit_code_t *jit, uint32_t jump){
      check_size(jit, 10);
      memcpy(jit->code_buf+jit->code_size, 
                    "\x8A\x06"                       // mov rax, [rsi]; load current cell value into rax    
                    "\x3C\x00"                   // cmp rax, 0; compare rax with 0 
                    "\x0F\x85",6);                          // jne lable_jump; jump if not equal
      jit->code_size += 10;

      int32_t offset = static_cast<int32_t>(jump - jit->code_size); // Calculate the offset for the jump
      memcpy(jit->code_buf+jit->code_size-4, &offset, 4); // Write the offset to the code buffer
      //return "lable_"+std::to_string(pc)+":\n mov rax, [rsi]\ncmp rax, 0\n jne lable_"+std::to_string(jump)+"\n";
    };
    
    void beqz(jit_code_t*jit){
      check_size(jit, 10);
      memcpy(jit->code_buf+jit->code_size, 
             "\x8A\x06"                       // mov rax, [rsi]; load current cell value into rax    
             "\x3C\x00"                   // cmp rax, 0; compare rax with 0 
             "\x0F\x84\x00\x00\x00\x00",10);       // je lable_jump; jump if equal
      jit->code_size += 10;
    };
};

#endif