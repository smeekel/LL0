#pragma once
#include <stdint.h>
#include "Lexer.h"
#include "Tokens.h"


namespace LL0
{
  class Node;

  class Parser
  {
  public:
    Parser  (class IStream*);
    ~Parser ();

  public:
    void test();

  protected:
    void  next      ();
    int   accept    (const Tokens type);

  private:
    Node*   pProgam               ();
    Node*   pStatements           ();
    Node*   pStatement            ();
    Node*   pIfStatement          ();
    Node*   pFunction             ();
    Node*   pParameterDefinition  ();
    Node*   pBlockStatement       ();
    Node*   pExpressionStatement  ();
    Node*   pExpression           ();
    Node*   pTerm                 ();
    Node*   pFactor               ();

    bool    isExpression          ();

  private:
    Lexer       lexer;
    SmartToken  token;
    Tokens      type;
  };

}
