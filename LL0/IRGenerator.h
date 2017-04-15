#pragma once
#include <map>
#include <string>
#include "Node.h"
#include "IRModule.h"


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
    void  gFunctionPushParams (const Node*, int& index);
    void  gReturn             (const Node*);
    
    int   gAssignment         (const Node*);
    int   gLiteral            (const Node*);
    int   gIdent              (const Node*);
    int   gPlus               (const Node*);
    int   gMinus              (const Node*);
    int   gMul                (const Node*);
    int   gDiv                (const Node*);

  protected:
    int   countParameters     (const Node*);

  private:
    IRModule  ir;

  };

}
