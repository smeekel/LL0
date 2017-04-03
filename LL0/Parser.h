#pragma once
#include <stdint.h>
#include "Lexer.h"
#include "Tokens.h"


namespace LL0
{

  class Parser
  {
  public:
    Parser  (class IStream*);
    ~Parser ();

  public:
    void test();

  protected:
    int   accept    (const Tokens type);
    int   acceptMany(const Tokens type, ...);
    void  next      ();

  private:
    class Node* pProgam     ();
    class Node* pExpression ();
    class Node* pTerm       ();
    class Node* pFactor     ();

  private:
    Lexer       lexer;
    SmartToken  token;
    Tokens      type;
  };

}
