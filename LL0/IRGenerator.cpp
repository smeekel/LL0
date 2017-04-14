#include "IRGenerator.h"

using namespace LL0;

IRGenerator::IRGenerator()
{
}

IRGenerator::~IRGenerator()
{
}

void IRGenerator::generate(SafeNode parseTree)
{
  parseTree->print();

  varIndex = 0;
  gTop(parseTree.get());
}

void IRGenerator::gTop(const Node* n)
{
  if( n->is(N_GLUE) )
  {
    gTop(n->getA());
    gTop(n->getB());
  }
  else if( n->is(N_VAR) )
  {
    gVar(n);
  }
  else
  {
    printf("N=%s\n", nodeTypeToString(n->getType()));
  }
}

void IRGenerator::gVar(const Node* n)
{
  const int index = ++varIndex;
  const Node* ident       = n->getA();
  const Node* expression  = n->getB();

  printf(".VAR $%i // %s\n", index, ident->getRaw());

  if( expression )
  {
    
  }
}

