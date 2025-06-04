SHELL = /bin/bash

CC = g++
CFLAGS = -Wall -Wextra -std=c++20

# source
MAIN = src/brainfuck_compiler.cpp
INTERFACE = src/architecture_interface.cpp
UTILS = src/utils.cpp
DEBUG = src/debugger.cpp

# Header 
UTILS_H = src/utils.hpp
DEBUG_H = src/debugger.hpp

OBJS = src/brainfuck_compiler.o src/architecture_interface.o src/utils.o src/debugger.o
TARGET = bc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	
src/brainfuck_compiler.o: $(MAIN) $(UTILS_H) $(DEBUG_H)
	$(CC) $(CFLAGS) -c $(MAIN) -o $@

src/architecture_interface.o: $(INTERFACE) $(UTILS_H)
	$(CC) $(CFLAGS) -c $(INTERFACE) -o $@

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