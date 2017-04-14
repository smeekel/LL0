#pragma once
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
    void gTop   (const Node*);
    void gGlue  (const Node*);
    void gVar   (const Node*);

  private:
    int varIndex;
  };

}
