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
To add a new architecture, you will need to:
1. Create a new file in the correct folder 
2. Implement the arch interface as described in the interfaces themselfs
3. Inside the utils.cpp file add the correct arch matching your architecture
4. if your arch is not already included just add the enum
5. compile the compiler and you are ready to go

### Benchmark

This benchmarks are kind of useless. My main intrest is trying to optimise the jit as much as I can

All benches are insite the bench folder, some are just to make sure the copiler works other test some specifics areas

| Test            	| 0.0   	| 1.0 	| 1.1   |
|-----------------	|-------	|-----	|----   |
| perf.bf         	| 3,90  	| 3,17  | 3,16  |
| pi-16.bf        	| 0,211 	| 0,200 | 0,200 |
| golden-ratio.bf 	| 0,132 	| 0,126	| 1,126 |
| yapi.bf           | 0,026     | 0,018 | 0,016 |
| long.bf           | 4,627     | 4,533 | 4,477 |

Version 0.0
    - without any major optimisation

Version 1.0
    - instead of allocating a buffer for the jit instruction then memcopy-ing those into the mmap the jit buffer is the mmaped region

Version 1.1
    - optimised the Lexer, instead of reling on reading char by char from the file everyting is copied in a char array


