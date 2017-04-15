#pragma once
#include <map>
#include "Symbol.h"


namespace LL0
{

  class SymbolTable
  {
  public:
    SymbolTable();

  public:
    void    scopePush       ();
    void    scopePop        ();
    Symbol  newSymbol       (const char* name);
    Symbol  findSymbol      (const char* name);
    Symbol  findLocalSymbol (const char* name);

    int     newVirtualIndex ();

  private:
    using SymbolPair = std::pair<std::string, Symbol>;
    std::multimap<std::string, Symbol> table;

    int virutalIndex;
    int scope;

  };

}
