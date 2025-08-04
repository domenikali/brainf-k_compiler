# brainf-ck_compiler
Simple Brainfuck compiler and JIT
==============================================

## Overview
This is a simple Brainfuck JIT compiler targeting RISC and CISC architectures. It takes Brainfuck code as input and compiles it to the appropriate assembly for your system.

## How to use
1. Clone the repository

2. Run the makefile:  
    ```sh
    make 
    ```

3. Compile your Brainfuck file:     
    ```sh
    ./bc <myCode.bf> 
    ```

4. For additional options, run: 
    ```sh
    ./bc -h
    ```

## Supported architectures:
### Compiler
- ARM32
- x86_64

### JIT Compiler
- x86_64

## Add an architecture
To add a new architecture, you will need to:
1. Create a new file in the correct folder 
2. Implement the arch interface as described in the interfaces themselves
3. Inside the utils.cpp file, add the correct arch matching your architecture
4. If your arch is not already included, just add it to the `CompilerArch` enum
5. Compile the compiler and you are ready to go

## Benchmark

These benchmarks are kind of useless. My main interest is trying to optimize the JIT as much as I can.

All benches are inside the bench folder. Some are just to make sure the compiler works; others test specific areas.

| Test            	| 0.0   	| 1.0 	| 1.1   | 1.2   | 1.3   |
|-----------------	|-------	|-----	|----   |----   |----   |
| perf.bf         	| 3.90  	| 3.17  | 3.16  | 2.96  | 2.313 |
| pi-16.bf        	| 0.211 	| 0.200 | 0.200 | 0.186 | 0.165 |
| golden-ratio.bf 	| 0.132 	| 0.126	| 0.126 | 0.125 | 0.122 |
| yapi.bf           | 0.026   | 0.018 | 0.016 | 0.016 | 0.016 |
| long.bf           | 4.627   | 4.533 | 4.477 | 1.603 | 0.743 |

**Version 0.0**
- Without any major optimization, just a standard instructions merging

**Version 1.0**
- Instead of allocating a buffer for the JIT instruction then memcpy-ing those into the mmap, the JIT buffer is the mmaped region

**Version 1.1**
- Optimized the Lexer. Instead of relying on reading char by char from the file, everything is copied into a char array

**Version 1.2**
- Optimized [+] and [-] to `mov [r], 0`

**Version 1.3**
- Optimised [->+<] and [-<+>] as `add to`, this implementation keep count of each `<` in order to count how many cells to move from

## Optimization Techniques

This chapter is all about how I designed the optimization techniques.

### DOD

#### Instruction structure
Starting from the first implementation, this structure holding necessary information about instructions for the compilation came down from 24 bytes to 8. This reduction in size helps with overhead and cache misses.

Originally, the instruction struct was very simple and intuitive:
```c++
typedef struct{
  InstructionType type;
  uint64_t count;
  uint64_t branch_address;
}Instruction
```
This produced a 24-byte structure with 7 bytes of padding.

By uniting count and branch_address, each used in different instructions (count for ADD, SUB, INC, DEC and branch_address used for BNEQ and BEQZ) into a single uint32_t "extra", I was able to bring the total down to 8 bytes with 3 bytes of padding:

```c++
typedef struct{
  InstructionType type;
  uint32_t extra; //used for branch_address and count
}Instruction
```

#### Branch Patches
By design, the compiler patches the branch addresses with the loop closure. Doing so requires less computing power by not iterating over the instructions multiple times to check for branching. 
(Yes the O is the same but, with larg programs this helps quite a bit)

### Assembly optimization

#### Merging Commands
In Brainfuck, each char means a single instruction. By merging all identical commands into a single one but with a different number, it's quite faster. Instead of `add reg, 1` n times, it's much faster to `add reg, n`.

#### Register reuse
Because of the architecture, especially for stdin and stdout operations, it's faster to use the `buf reg` as a tape pointer. Also, by architecture, Brainfuck can print at most 1 char at a time, so we can preload the `size reg` with 1. Doing so, each stdin and stdout operation requires 2 fewer instructions, 40% fewer instructions per block stdin & stdout.

### Passes

In Brainfuck, it's common to use macros of commands as specific instructions that are not natively available. These passes aim to drastically reduce the number of instructions and cycles used to improve performance in both time and memory.

#### MOV 0
In Brainfuck, [-] & [+] are commonly used to zero the current cell. However, this is easy to spot by passing through the instructions and checking for the distance of the brackets.

Because of the merging, every possible `mov 0` configuration can be detected by noticing a distance of 1 between the brackets (because of merging, [++] will be stored as [+] with the extra instruction knowledge that the `+` operation is repeated 2 times).

Each time the pass recognizes this pattern, it just removes the entire loop and changes them to a `mov 0`.