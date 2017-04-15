#pragma once
#include "SymbolTable.h"

namespace LL0
{

  class IRModule
  {
  public:
    IRModule();

  public:
    int newVirtualIndex();

  public:
    SymbolTable sym;

  private:

  };

}
