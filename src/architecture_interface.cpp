#include <iostream>

/**
 * @file Architecture_Interface.h
 * @brief This file defines the Architecture_Interface class, which serves as an interface for different architectures.
 * 
 * The Architecture_Interface class provides pure virtual functions that must be implemented by any derived class.
 * These functions represent operations that can be performed in a specific architecture, such as starting and ending
 * a program, performing arithmetic operations, and managing pointers.
 * 
 * @note
 * This file is part of a larger project that compiles Brainfuck code into various architectures.
 * The project is designed to be modular, allowing for easy addition of new architectures by implementing this interface.
 */
class ArchitectureInterface {
  public:
    virtual ~ArchitectureInterface() = default;

    /**
     * @brief virtual function to start a program.
     * this function returns a string that represents the start of a program in the specific architecture.
     * @return std::string representing the start of a program.
    */
    virtual std::string proStart()=0;
    /**
     * @brief virtual function to end a program.
     * this function returns a string that represents the end of a program in the specific architecture.
     * @return std::string representing the end of a program.
    */
    virtual std::string proEnd()=0;
    /**
     * @brief virtual function add one to the current pointer value.
     * this function returns a string that represents the addition operation in the specific architecture.
     * @return std::string representing the addition operation.
    */
    virtual std::string add()=0;
    /**
     * @brief virtual function to subtract one from the current pointer value.
     * this function returns a string that represents the subtraction operation in the specific architecture.
     * @return std::string representing the subtraction operation.
    */
    virtual std::string subtract()=0;
    /**
     * @brief virtual function to print the current pointer value.
     * this function returns a string that represents the print operation in the specific architecture.
     * @return std::string representing the print operation.
    */
    virtual std::string print()=0;
    /**
     * @brief virtual function to read input into the current pointer value.
     * this function returns a string that represents the input operation in the specific architecture.
     * @return std::string representing the input operation.
    */
    virtual std::string input()=0;
    /**
     * @brief virtual function to increment the current pointer.
     * this function returns a string that represents the increment pointer operation in the specific architecture.
     * @return std::string representing the increment pointer operation.
    */
    virtual std::string incrementPointer()=0;
    /**
     * @brief virtual function to decrement the current pointer.
     * this function returns a string that represents the decrement pointer operation in the specific architecture.
     * @return std::string representing the decrement pointer operation.
    */
    virtual std::string decrementPointer()=0;
    /**
     * @brief virtual function to start a cycle.
     * this function returns a string that represents the start of a cycle in the specific architecture.
     * @return std::string representing the start of a cycle.
    */
    virtual std::string startCycle()=0;
    /**
     * @brief virtual function to end a cycle.
     * this function returns a string that represents the end of a cycle in the specific architecture.
     * @return std::string representing the end of a cycle.
    */
    virtual std::string endCycle()=0;
};