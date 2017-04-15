#include <cstdio>
#include <cstdarg>
#include "Parser.h"
#include "Exception.h"
#include "Node.h"

using namespace LL0;

//#define ACCEPT(...) acceptMany(__VA_ARGS__, -1)
#define TYPE (type)

#define ACCEPT(x) accept(x)

#define EXPECT(x, c) \
{ \
  if( TYPE!=(x) ) \
    throw EXCEPTION("(%d:%d) Syntax error: expected " #c, token->getLine(), token->getColumn()); \
  else \
    next(); \
}

#define PARSE_EXCEPTION(message, ...) EXCEPTION("(%d:%d) " message, token->getLine(), token->getColumn(), __VA_ARGS__)


Parser::Parser(class IStream* input)
  : lexer(input)
{
  next();
}

Parser::~Parser()
{
}


SafeNode Parser::generate()
{
  return SafeNode( pProgam() );
}


Node* Parser::pProgam()
{
  Node* parent = NULL;

  parent = pStatements();

  return parent;
}

Node* Parser::pStatements()
{
  Node* parent = NULL;

  try
  {
    while( true )
    {
      Node* statement = pStatement();
      if( !statement ) break;

      parent = (parent) ? new Node(N_GLUE, parent, statement) : statement;
    }
  }
  catch( ... )
  {
    SAFE_DELETE(parent);
    throw;
  }

  return parent;
}

Node* Parser::pStatement()
{
  if( ACCEPT(T_IF) )
  {
    return pIfStatement();
  }
  else if( ACCEPT(T_FUNCTION) )
  {
    return pFunction();
  }
  else if( ACCEPT(T_RETURN) )
  {
    return pReturnStatement();
  }
  else if( ACCEPT(T_VAR) )
  {
    return pVariableDefinition();
  }
  else if( ACCEPT(T_LBRACE) )
  {
    return pBlockStatement();
  }
  else if( isExpression() )
  {
    return pExpressionStatement();
  }

  return NULL;
}

Node* Parser::pExpressionStatement()
{
  Node* expression = NULL;
  
  
  try
  {
    expression = pAssignmentExpression();
    EXPECT(T_SEMICOLON, ";");
  }
  catch( ... )
  {
    SAFE_DELETE(expression);
    throw;
  }

  return expression;
}

Node* Parser::pBlockStatement()
{
  Node* parent = NULL;

  try
  {
    parent = pStatements();
    EXPECT(T_RBRACE, "}");
  }
  catch( ... )
  {
    SAFE_DELETE(parent);
    throw;
  }

  return parent;
}

Node* Parser::pReturnStatement()
{
  Node* expression = NULL;

  try
  {
    if( isExpression() )
      expression = pAssignmentExpression();

    EXPECT(T_SEMICOLON, ";");
  }
  catch( ... )
  {
    SAFE_DELETE(expression);
    throw;
  }

  return new Node(N_RETURN, expression);
}

Node* Parser::pVariableDefinition()
{
  Node* name        = NULL;
  Node* expression  = NULL;

  try
  {
    if( TYPE!=T_IDENT )
      throw PARSE_EXCEPTION("Expected variable name");
    name = new Node(N_IDENT);
    name->setToken(*token);
    next();

    if( ACCEPT(T_ASSIGNMENT) )
    {
      expression = pExpression();
    }

    EXPECT(T_SEMICOLON, ';');
  }
  catch( ... )
  {
    SAFE_DELETE(name);
    SAFE_DELETE(expression);
    throw;
  }

  return new Node(N_VAR, name, expression);
}

Node* Parser::pFunction()
{
  Node* name    = NULL;
  Node* params  = NULL;
  Node* body    = NULL;

  try
  {
    if( TYPE==T_IDENT )
    {
      name = new Node(N_IDENT);
      name->setToken(*token);
      next();
    }
    else
    {
      name = new Node(N_ANONYMOUS);
    }

    EXPECT(T_LPREN, "(");
    params = pParameterDefinition();
    
    EXPECT(T_LBRACE, "{");
    body = pBlockStatement();
    
  }
  catch( ... )
  {
    SAFE_DELETE(name);
    SAFE_DELETE(params);
    SAFE_DELETE(body);
    throw;
  }

  return new Node(N_FUNCTION, name, params, body);
}


Node* Parser::pParameterDefinition()
{
  Node* parent = NULL;

  try
  {
    while( true )
    {
      if( TYPE==T_IDENT )
      {
        Node* ident = new Node(N_IDENT);
        ident->setToken(*token);
        parent = parent ? new Node(N_GLUE, parent, ident) : ident ;
        next();

        ACCEPT(T_COMMA);
      }
      else if( ACCEPT(T_RPREN) )
      {
        break;
      }
      else
      {
        throw PARSE_EXCEPTION("Syntax error in parameter definition");
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

Node* Parser::pIfStatement()
{
  Node* condition = NULL;
  Node* onTrue    = NULL;
  Node* onFalse   = NULL;

  try
  {
    EXPECT(T_LPREN, "(");
    condition = pExpression();
    EXPECT(T_RPREN, ")");
    
    onTrue = pStatement();

    if( ACCEPT(T_ELSE) )
      onFalse = pStatement();

  }
  catch( ... )
  {
    SAFE_DELETE(condition);
    SAFE_DELETE(onTrue);
    SAFE_DELETE(onFalse);
    throw;
  }

  return new Node(N_IF, condition, onTrue, onFalse);
}

Node* Parser::pAssignmentExpression()
{
  Node* parent = NULL;

  try
  {
    parent = pExpression();

    if( TYPE==T_ASSIGNMENT )
    {
      next();
      return new Node(N_ASSIGNMENT, parent, pExpression());
    }
    else
    {
      return parent;
    }
  }
  catch( ... )
  {
    SAFE_DELETE(parent);
    throw;
  }
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

  switch( TYPE )
  {
    case T_NUMBER:
    case T_BIN_NUMBER:
    case T_HEX_NUMBER:
    case T_OCT_NUMBER:
    {
      n = new Node(N_N_LITERAL);
      n->setToken(*token);
      next();
      break;
    }

    case T_IDENT:
    {
      SmartToken peek = lexer.peek();

      if( peek->getType()==T_LPREN )
      {
        //
        // Function call
        //
        n = pFunctionCall();
      }
      else
      {
        //
        // Variable
        //
        n = new Node(N_IDENT);
        n->setToken(*token);
        next();
      }
      break;
    }

    case T_LPREN:
    {
      next();
      n = pAssignmentExpression();
      if( TYPE!=T_RPREN )
      {
        SAFE_DELETE(n);
        throw EXCEPTION("(%d:%d) Missing closing paren", token->getLine(), token->getColumn());
      }
      next();
      break;
    }

    default:
    {
      throw EXCEPTION("(%d:%d) Unexpected input", token->getLine(), token->getColumn());
    }
  }

  return n;
}

Node* Parser::pFunctionCall()
{
  Node* name = NULL;
  Node* parameters = NULL;

  try
  {
    name = new Node(N_IDENT);
    name->setToken(*token);
    next();

    EXPECT(T_LPREN, "(");
    parameters = pExpressionList();
    EXPECT(T_RPREN, ")");
  }
  catch( ... )
  {
    SAFE_DELETE(name);
    SAFE_DELETE(parameters);
    throw;
  }

  return new Node(N_CALL, name, parameters);
}

Node* Parser::pExpressionList()
{
  Node* list = NULL;

  try
  {
    while( true )
    {
      if( TYPE==T_RPREN )
      {
        break;
      }
      else if( isExpression() )
      {
        Node* expr = pAssignmentExpression();
        list = list ? new Node(N_NEXT, list, expr) : expr ;
        ACCEPT(T_COMMA);
      }
    }
  }
  catch( ... )
  {
    SAFE_DELETE(list);
    throw;
  }

  return list;
}


bool Parser::isExpression()
{
  switch( TYPE )
  {
    case T_NUMBER:
    case T_BIN_NUMBER:
    case T_HEX_NUMBER:
    case T_OCT_NUMBER:
    case T_LPREN:
    case T_IDENT:
      return true;

    default: return false;
  }
}

int Parser::accept(const Tokens type)
{
  if( type==this->type )
  {
    next();
    return 1;
  }

  return 0;
}

void Parser::next()
{
  token = lexer.next();
  type  = token->getType();
}

