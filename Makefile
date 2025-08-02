SHELL = /bin/bash

CC = g++
CFLAGS = -Wall -Wextra -std=c++20 -ggdb -O3

# Source files (.cpp)
MAIN = src/brainfuck_compiler.cpp
UTILS = src/utils.cpp
DEBUG = src/debugger.cpp
LEX = src/lexer.cpp

# Header files (.hpp) - solo per dipendenze
UTILS_H = src/utils.hpp
DEBUG_H = src/debugger.hpp
LEX_H = src/lexer.hpp
ARCH_INTERFACE_H = src/architecture_interface.hpp
ARM32 = src/comp_arch/arm32.hpp
X86 = src/jit_arch/x86_jit.hpp


# Object files
OBJS = src/brainfuck_compiler.o src/utils.o src/debugger.o src/lexer.o
TARGET = bc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Dipendenze corrette con tutti gli header necessari
src/brainfuck_compiler.o: $(MAIN) $(UTILS_H) $(DEBUG_H) $(ARCH_INTERFACE_H) $(ARM32_H) $(X86_H) $(LEX_H)
	$(CC) $(CFLAGS) -c $(MAIN) -o $@

src/lexer.o: $(LEX) $(LEX_H) $(UTILS_H)
	$(CC) $(CFLAGS) -c $(LEX) -o $@

src/utils.o: $(UTILS) $(UTILS_H)
	$(CC) $(CFLAGS) -c $(UTILS) -o $@

src/debugger.o: $(DEBUG) $(UTILS_H) $(DEBUG_H)
	$(CC) $(CFLAGS) -c $(DEBUG) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f src/*.o
	rm -f *.o
	rm -f *.dbg
	rm -f *.asm
	rm -f *.bin

.PHONY: all clean run