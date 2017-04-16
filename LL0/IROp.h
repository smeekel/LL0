#pragma once
#include <string>


namespace LL0
{

  enum IROps
  {
    NOP = 0,
    
    PUSH,
    POP,
    MOV,
    SMOV,
    LOADK,
    CALL,
    RET,
    JMP,
    JMPF,

    ADD,
    SUB,
    MUL,
    DIV,
  };

  class IROp
  {
  public:
    IROp(const IROps op)
    {
      this->op  = op;
      this->A   = 0;
      this->B   = 0;
      this->C   = 0;
    }

  public:
    IROps op;
    std::string raw;

    int   A;
    int   B;
    int   C;
  };

}
