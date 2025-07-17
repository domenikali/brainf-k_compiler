#ifndef JIT_ARCH_INTERFACE_H
#define JIT_ARCH_INTERFACE_H
#include <cstdint>
#include <cstring>
#include <iostream>

/**
 * @brief this structure represents the JIT code buffer.
 * It contains a pointer to the code buffer, the size of the code, and the size of the memory allocated for the code.
 */
typedef struct{
  unsigned char *code_buf;
  size_t code_size;
  size_t memory_size;
}jit_code_t;

/**
 * @brief this function checks if the JIT code buffer has enough space for the given size.
 * if not, it prints an error message and returns false.
 */
inline bool check_size(jit_code_t*jit, size_t cs){
  if (jit->code_size + cs >= jit->memory_size) {
    std::cerr << "Error: JIT code buffer overflow." << std::endl;
    return false;
  }
  return true;
}

/**
 * @brief JITInterface is an abstract class that defines the interface for JIT compilation.
 * It contains pure virtual functions that must be implemented by derived classes.
 * These functions represent operations that can be performed in a specific architecture, such as starting and ending a program,
 * performing arithmetic operations, and managing pointers.
 * @note
 * To implement a new architecture, you need to derive a class from JITInterface and implement all the pure virtual functions.
 * This allows the JIT compiler to generate code for different architectures without changing the core compilation logic
 * each instruction need to copy the machine code to the jit_code_t structure, in particular the code_buf and code_size fields.
 * The code_buf is a pointer to the code buffer, and code_size is the size of the code buffer.
 * The memory_size field is the size of the memory allocated for the code buffer, it's imperative to check if the code_size + cs is less than memory_size before copying the code.
 * The interface provides a bool check_size function to help in this process
 */
class JITInterface {
  public:
    
  /**
   * @brief Virtual method to start a JIT program.
   * This function takes a pointer to a JIT code structure and initializes the program start.
   * It's main function is to prepare the register that will handle the memory pointer.
   * @param jit Pointer to the JIT code structure.  
  */
  virtual void proStart(jit_code_t *jit)=0;
    
  /**
   * @brief Virtual method to end a JIT program.
   * This function takes a pointer to a JIT code structure and finalizes the program end.
   * It typically includes the system call to exit the program.
   * @param jit Pointer to the JIT code structure.
  */
  virtual void proEnd(jit_code_t *jit)=0;
  
  /**
   * @brief Virtual method to increment the current cell value.
   * This function takes a pointer to a JIT code structure and increment the current cell to the value of count.
   * In brainfuck each cell can hold up to 255, so the count is a uint8_t and the value is supposed to wrap.
   * @param jit Pointer to the JIT code structure.
   * @param count The value to add to the current cell.
   */
  virtual void add(jit_code_t *jit,uint8_t count)=0;
    
  /**
   * @brief Virtual method to decrement the current cell value.
   * This function takes a pointer to a JIT code structure and decrement the current cell to the value of count.
   * In brainfuck each cell can hold up to 255, so the count is a uint8_t and the value is supposed to wrap.
   * @param jit Pointer to the JIT code structure.
   * @param count The value to subtract to the current cell.
   */
  virtual void sub(jit_code_t*jit,uint8_t count)=0;
    
  /**
   * @brief Virtual method to print the current cell as ASCII char.
   * This function takes a pointer to a JIT code structure and prints the current cell value.
   * It typically includes the system call to write to stdout.
   * @param jit Pointer to the JIT code structure.
   */
  virtual void output(jit_code_t *jit)=0;
    
  /**
   * @brief Virtual method to take from input a value and store it in the current cell.
   * This function takes a pointer to a JIT code structure and take a value from input then stores it in the current cell value.
   * It typically includes the system call to read from stdout.
   * @param jit Pointer to the JIT code structure.
   */
  virtual void input(jit_code_t *jit)=0;
    
  /**
   * @brief Virtual method to increment the current pointer.
   * This function takes a pointer to a JIT code structure and increments the current pointer by the value of count.
   * While for most code a uint8_t is enough it's advisable to use a uint32_t to avoid overflow issues.
   * @param jit Pointer to the JIT code structure.
   * @param count The value to increment the current pointer.
   */
  virtual void inc(jit_code_t *jit,uint8_t count)=0;
    
  /**
   * @brief Virtual method to decrement the current pointer.
   * This function takes a pointer to a JIT code structure and decrements the current pointer by the value of count.
   * While for most code a uint8_t is enough it's advisable to use a uint32_t to avoid overflow issues.
   * @param jit Pointer to the JIT code structure.
   * @param count The value to decrement the current pointer.
   */
  virtual void dec(jit_code_t *jit,uint8_t count)=0;
  
  /**
   * @brief Virtual method to branch if the current cell is not equal to zero.
   * This function takes a pointer to a JIT code structure and branches to the instruction at jump if the current cell is not equal to zero.
   * It typically includes the logic to compare the current cell value with zero and jump to the specified instruction.
   * @param jit Pointer to the JIT code structure.
   * @param jump The address to jump to if the current cell is not equal to zero.
   * @note The logic used into the jit compiler is to assign the jump address of the bneq instruction during the compilation of the beqz instruction.
   */
  virtual void bneq(jit_code_t *jit, uint32_t jump)=0;
    
  /**
   * @brief Virtual method to branch if the current cell is equal to zero.
   * This function takes a pointer to a JIT code structure and branches to the instruction at jump if the current cell is equal to zero.
   * @param jit Pointer to the JIT code structure.
   * @param jump The address to jump to if the current cell is not equal to zero.
   * @note The logic used into the jit compiler is to assigne the jump address of the beqz instruction during the compilation of the bneq instruction, because during the beqz instruction the jump address is not known yet.
   */
  virtual void beqz(jit_code_t*jit)=0;
};

#endif