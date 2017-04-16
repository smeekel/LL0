#pragma once
#include <vector>
#include "SymbolTable.h"
#include "IROp.h"


namespace LL0
{

  class IRModule
  {
  public:
    IRModule();

  public:
    int   newVirtualIndex();
    void  addOp(IROp);
    void  addOp(const IROps);
    void  addOp(const IROps, const int);
    void  addOp(const IROps, const int, const int);
    void  addOp(const IROps, const int, const int, const int);

    void  print();

  protected:
    void  print_op(const IROp&);

  public:
    SymbolTable sym;

  private:
    std::vector<IROp> ops;

  };

}
