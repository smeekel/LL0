#pragma once
#include <cstdint>
#include "Lexer.h"
#include "Tokens.h"
#include "Node.h"


namespace LL0
{
  class Node;

  class Parser
  {
  public:
    Parser  (class IStream*);
    ~Parser ();

  public:
    SafeNode generate();

  protected:
    void  next      ();
    int   accept    (const Tokens type);

  private:
    Node*   pProgam               ();
    Node*   pStatements           ();
    Node*   pStatement            ();
    Node*   pIfStatement          ();
    Node*   pFunction             ();
    Node*   pVariableDefinition   ();
    Node*   pParameterDefinition  ();
    Node*   pBlockStatement       ();
    Node*   pReturnStatement      ();
    Node*   pAssignmentExpression ();
    Node*   pExpressionStatement  ();
    Node*   pExpression           ();
    Node*   pExpressionList       ();
    Node*   pTerm                 ();
    Node*   pFactor               ();
    Node*   pFunctionCall         ();

    bool    isExpression          ();

  private:
    Lexer       lexer;
    SmartToken  token;
    Tokens      type;
  };

}
