#pragma once
#include <map>
#include <string>
#include "Node.h"


namespace LL0
{

  class IRGenerator
  {
  public:
    IRGenerator();
    ~IRGenerator();

  public:
    void generate(SafeNode parseTree);

  protected:
    void  gTop    (const Node*);
    void  gGlue   (const Node*);
    void  gVar    (const Node*);
    int   gEval   (const Node*);

  protected:
    class Symbol
    {
    public:
      Symbol()
      {
        temp = 0;
      }

    public:
      std::string name;
      int         temp;

    public:
      int   getTemp () const { return temp; }
      bool  isNull  () const { return temp==0; }

      static Symbol null() { return Symbol(); }
    };

  protected:
    Symbol  newSymbol (const char* name);
    Symbol  findSymbol(const char* name);

  private:
    using SymbolPair = std::pair<std::string, Symbol>;
    std::map<std::string, Symbol> symbolTable;
    int varIndex;
  };

}
