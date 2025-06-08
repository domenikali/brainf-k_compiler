SHELL = /bin/bash

CC = g++
CFLAGS = -Wall -Wextra -std=c++20

# Source files (.cpp)
MAIN = src/brainfuck_compiler.cpp
UTILS = src/utils.cpp
DEBUG = src/debugger.cpp

# Header files (.hpp) - solo per dipendenze
UTILS_H = src/utils.hpp
DEBUG_H = src/debugger.hpp
ARCH_INTERFACE_H = src/architecture_interface.hpp
ARM32_H = src/architectures/arm32.hpp

# Object files - RIMUOVI architecture_interface.o perché non esiste più il .cpp
OBJS = src/brainfuck_compiler.o src/utils.o src/debugger.o
TARGET = bc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Dipendenze corrette con tutti gli header necessari
src/brainfuck_compiler.o: $(MAIN) $(UTILS_H) $(DEBUG_H) $(ARCH_INTERFACE_H) $(ARM32_H)
	$(CC) $(CFLAGS) -c $(MAIN) -o $@

src/utils.o: $(UTILS) $(UTILS_H)
	$(CC) $(CFLAGS) -c $(UTILS) -o $@

src/debugger.o: $(DEBUG) $(UTILS_H) $(DEBUG_H)
	$(CC) $(CFLAGS) -c $(DEBUG) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f src/*.o
	rm -f *.dbg
	rm -f *.asm

.PHONY: all clean run