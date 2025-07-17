#ifndef X86JIT_H
#define X86JIT_H
#include "../JIT_arch_iterface.hpp"

#define BRANCH_ADDRESS_SIZE 4

class X86JIT:public JITInterface {
  public:
    X86JIT(uint8_t *branch_address_size){
      *branch_address_size =BRANCH_ADDRESS_SIZE;
    }
    void proStart(jit_code_t *jit) override{
      check_size(jit, 8);
      memcpy(jit->code_buf + jit->code_size, 
        "\x48\x89\xFE"                          // mov rsi,rdi; move memory pointer to rsi
        "\xBF\x01\x00\x00\x00", 8);             // mov edi, 1; (signle byte) for optimisation purposes during read and write from and to stdout
      jit->code_size += 8;
    };
    
    void proEnd(jit_code_t *jit)override{
      check_size(jit, 12);
      memcpy(jit->code_buf + jit->code_size, 
              "\xB8\x3C\x00\x00\x00"            // mov eax, 60; (sys_exit)
              "\xBf\x00\x00\x00\x00"            // mov edi, 0; (exit code 0)
              "\x0F\x05",12);                   // syscall           
      jit->code_size += 12;
    };
    
    void add(jit_code_t *jit,uint8_t count)override{
      check_size(jit, 9);
      memcpy(jit->code_buf + jit->code_size, 
              "\x8A\x06"                        // mov al, [rsi]; current cell value into al
              "\x04", 3);                       // add al, count; add the count to al
      memcpy(jit->code_buf+jit->code_size+3, &count, 1);; // hex value
      memcpy(jit->code_buf + jit->code_size + 4, 
              "\x88\x06", 3);                   // mov [rsi], al; store back to tape
      jit->code_size += 6;

    };
    
    void sub(jit_code_t*jit,uint8_t count)override{
      check_size(jit, 6);
      memcpy(jit->code_buf + jit->code_size,
              "\x8A\x06"                        // mov al, [rsi]; load current cell value into al
              "\x2C",3);                        // sub al, count; sub the count to al    
      memcpy(jit->code_buf+jit->code_size+3, &count, 1); //hex value    
      memcpy(jit->code_buf+jit->code_size+4,
              "\x88\x06",3);                    // mov [rsi], al; store back to tape
      jit->code_size += 6;
    };
    
    void output(jit_code_t *jit)override{
      check_size(jit, 12);
      memcpy(jit->code_buf+jit->code_size, 
              "\xB8\x01\x00\x00\x00"             // mov eax, 1; (sys_write)  
              "\xBA\x01\x00\x00\x00"             // mov edx, 1; stdout file descriptor
              "\x0F\x05",12);                    // syscall; syscall
      jit->code_size += 12;
    };
    
    void input(jit_code_t *jit)override{
      check_size(jit, 12);
      memcpy(jit->code_buf+jit->code_size,
             "\xB8\x00\x00\x00\x00"               // mov eax, 0; (sys_read)  
             "\xBA\x01\x00\x00\x00"               // mov edx, 1; number of bytes to write
             "\x0F\x05",12);                      // syscall; syscall   
      jit->code_size += 12;

    };
    
    void inc(jit_code_t *jit,uint8_t count)override{
      check_size(jit, 4);
      memcpy(jit->code_buf+jit->code_size,
              "\x48\x83\xC6",3);                  // add rsi, count; increment tape pointer
      memcpy(jit->code_buf+jit->code_size+3, &count, 1);
      jit->code_size += 4;

    };
    
    void dec(jit_code_t *jit,uint8_t count)override{
      check_size(jit, 4);
      memcpy(jit->code_buf+jit->code_size,
              "\x48\x83\xEE",3);                  // sub rsi, count; decrement tape pointer
      memcpy(jit->code_buf+jit->code_size+3, &count, 1);
      jit->code_size += 4;
  };
    
    void bneq(jit_code_t *jit, uint32_t jump)override{
      check_size(jit, 10);
      memcpy(jit->code_buf+jit->code_size, 
                    "\x8A\x06"                   // mov al, [rsi]; load current cell value into rax    
                    "\x3C\x00"                   // cmp al, 0; compare rax with 0 
                    "\x0F\x85",6);               // jne lable_jump; jump if not equal
      jit->code_size += 10;
      // Calculate the offset for the jump
      int32_t offset = static_cast<int32_t>(jump - jit->code_size);
      // Write the offset to the code buffer
      memcpy(jit->code_buf+jit->code_size-BRANCH_ADDRESS_SIZE, &offset, BRANCH_ADDRESS_SIZE); 
    };
    
    void beqz(jit_code_t*jit)override{
      check_size(jit, 10);
      memcpy(jit->code_buf+jit->code_size, 
             "\x8A\x06"                           // mov al, [rsi]; load current cell value into rax    
             "\x3C\x00"                           // cmp al, 0; compare rax with 0 
             "\x0F\x84\x00\x00\x00\x00",10);      // je lable_jump; jump if equal
      jit->code_size += 10;
    };
};

#endif