#ifndef ARM32_H
#define ARM32_H
#include "../architecture_interface.hpp"
class ARM32: public ArchitectureInterface {
  public:
    ARM32(){
      std::cout<<"arm32 architecture"<<std::endl;
    };

    std::string proStart()override{
      return "";
    };
    
    virtual std::string proEnd()override{
      return "";
    };
    
    virtual std::string add(uint8_t count)override{
      return "";
    };
    
    virtual std::string sub(uint8_t count)override{
      return "";
    };
    
    virtual std::string output(uint8_t count)override{
      return "";
    };
    
    virtual std::string input(uint8_t count)override{
      return "";
    };
    
    virtual std::string inc(uint8_t count)override{
      return "";
    };
    
    virtual std::string dec(uint8_t count)override{
      return "";
    };
    
    virtual std::string bneq()override{
      return "";
    };
    
    virtual std::string beqz()override{
      return "";
    };
          
};

#endif