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
    void  gBlock              (const Node*);
    void  gGlue               (const Node*);
    void  gVar                (const Node*);
    int   gEval               (const Node*);
    void  gPushCallParameters (const Node*);
    int   gCall               (const Node*);
    int   gIf                 (const Node*);
    void  gFunction           (const Node*);
    void  gReturn             (const Node*);
    
    int   gAssignment         (const Node*);
    int   gLiteral            (const Node*);
    int   gIdent              (const Node*);
    int   gPlus               (const Node*);
    int   gMinus              (const Node*);
    int   gMul                (const Node*);
    int   gDiv                (const Node*);

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
    Symbol  newSymbol       (const char* name);
    Symbol  findSymbol      (const char* name);
    int     countParameters (const Node*);

  private:
    using SymbolPair = std::pair<std::string, Symbol>;
    std::map<std::string, Symbol> symbolTable;
    int varIndex;

  };

}
