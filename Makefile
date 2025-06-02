SHELL = /bin/bash

CC = g++
CFLAGS = -Wall -Wextra -std=c++20

MAIN = src/brainfuck_compiler.cpp
INTERFACE= src/Architecture_Interface.cpp 

OBJS = $(MAIN:.cpp=.o) $(INTERFACE:.cpp=.o) 
TARGET = bc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f src/*.o

.PHONY: all clean
