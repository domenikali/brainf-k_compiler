# brainf-k_compiler
Simple Brainfuck compiler for RISC architectures
==============================================

### Overview
This is a simple Brainfuck compiler targeting RISC architectures. It takes Brainfuck code as input and compiles it to the appropriate assembly for your system.

The goal is to build a multi-target compiler that automatically detects the host CPU architecture and generates the correct assembly code accordingly.

### How to use
1. Clone the repository

2. Run the makefile:  
    ``` bash
    make 
    ```


3. Compile your Brainfuck file:     
    ```
    bc <myCode.bf> 
    ```

4. For additional options, run: 
    ```
    bc -h
    ```
 

### Supported architectures:

 ARM32
 

### Add architecture

