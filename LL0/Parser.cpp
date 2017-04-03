#include <cstdio>
#include <cstdarg>
#include "Parser.h"
#include "Exception.h"
#include "Node.h"

using namespace LL0;

#define ACCEPT(...) acceptMany(__VA_ARGS__, -1)
#define TYPE (type)

Parser::Parser(class IStream* input)
  : lexer(input)
{
  next();
}

Parser::~Parser()
{
}


void Parser::test()
{
  Node* root = pProgam();
  
  root->print();

  return;
}


Node* Parser::pProgam()
{
  Node* parent = NULL;

  try
  {
    while( !accept(T_EOF) )
    {
      Node* child = new Node(N_EXPRESSION, pExpression());
      parent = (parent) ? new Node(N_GLUE, parent, child) : child ;
    }
  }
  catch( ... )
  {
    SAFE_DELETE(parent);
    throw;
  }

  return parent;
}

Node* Parser::pExpression()
{
  Node* parent = pTerm();
  
  try
  {
    while( true )
    {
      if( TYPE==T_PLUS )
      {
        next();
        parent = new Node(N_PLUS, parent, pTerm());
      }
      else if( TYPE==T_MINUS )
      {
        next();
        parent = new Node(N_MINUS, parent, pTerm());
      }
      else
      {
        break;
      }
    }
  }
  catch( ... )
  {
    SAFE_DELETE(parent);
    throw;
  }

  return parent;
}

Node* Parser::pTerm()
{
  Node* parent = pFactor();

  try
  {
    while( true )
    {
      if( TYPE==T_MUL )
      {
        next();
        parent = new Node(N_MUL, parent, pFactor());
      }
      else if( TYPE==T_DIV )
      {
        next();
        parent = new Node(N_DIV, parent, pFactor());
      }
      else
      {
        break;
      }
    }
  }
  catch( ... )
  {
    SAFE_DELETE(parent);
    throw;
  }

  return parent;
}

Node* Parser::pFactor()
{
  Node* n = NULL;

  if( ACCEPT(T_NUMBER, T_BIN_NUMBER, T_HEX_NUMBER, T_OCT_NUMBER) )
  {
    n = new Node(N_N_LITERAL);
    n->setToken(*token);
    next();
  }
  else if( TYPE==T_LPREN )
  {
    next();
    n = pExpression();
    if( TYPE!=T_RPREN )
    {
      SAFE_DELETE(n);
      throw EXCEPTION("(%d:%d) Missing closing paren", token->getLine(), token->getColumn());
    }
    next();
  }
  else
  {
    throw EXCEPTION("(%d:%d) Unexpected input", token->getLine(), token->getColumn());
  }

  return n;
}

/*
Node* Parser::pExpression()
{
  Node* parent = new Node(N_EXPRESSION);

  if( TYPE==T_PLUS || TYPE==T_MINUS )
  {
    printf("+/-\n");
    next();
  }

  pTerm();

  while( TYPE==T_PLUS || TYPE==T_MINUS )
  {
    printf("+/-\n");
    next();
    pTerm();
  }
}

Node* Parser::pTerm()
{
  pFactor();
  while( TYPE==T_MUL || TYPE==T_DIV )
  {
    printf("* / \\\n");
    next();
    pFactor();
  }
}

Node* Parser::pFactor()
{
  if( ACCEPT(T_NUMBER, T_BIN_NUMBER, T_HEX_NUMBER, T_OCT_NUMBER) )
  {
    printf("number [%s]\n", token->getText());
    next();
  }
  else if( TYPE==T_LPREN )
  {
    pExpression();
    if( TYPE!=T_RPREN )
      throw EXCEPTION("(%d:%d) Missing closing paren", token->getLine(), token->getColumn());
  }
  else
  {
    throw EXCEPTION("(%d:%d) Unexpected input", token->getLine(), token->getColumn());
  }
}
*/



int Parser::acceptMany(const Tokens type, ...)
{
  va_list args;

  const Tokens compare = token->getType();
  if( compare==type )
    return 1;

  int returnValue = 0;
  va_start(args, type);
  for( ;; )
  {
    const Tokens t = va_arg(args, const Tokens);
    if( t==compare )
    {
      returnValue = 1;
      break;
    }
    else if( t==-1 )
    {
      break;
    }
  }
  va_end(args);

  return returnValue;
}

int Parser::accept(const Tokens type)
{
  if( token->getType()==type )
  {
    return 1;
  }

  return 0;
}

void Parser::next()
{
  token = lexer.next();
  type  = token->getType();
}

