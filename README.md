# brainf-k_compiler
Simple Brainfuck compiler and JIT
==============================================

### Overview
This is a simple Brainfuck JIT compiler targeting RISC and CISC architectures. It takes Brainfuck code as input and compiles it to the appropriate assembly for your system.


### How to use
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

 

### Supported architectures:
#### Compiler

- ARM32
 
- x86_64

#### JIT Compiler
- x86_64
 


### Add architecture

