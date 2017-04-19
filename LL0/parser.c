#include "parser.h"
#include "tokens.h"


static Node*  pStatements           (ParserState*);
static Node*  pStatement            (ParserState*);
static Node*  pExpressionStatement  (ParserState*);
static Node*  pBlockStatement       (ParserState*);
static Node*  pReturnStatement      (ParserState*);
static Node*  pVariableDefinition   (ParserState*);
static Node*  pFunction             (ParserState*);
static Node*  pParameterDefinition  (ParserState*);
static Node*  pIfStatement          (ParserState*);
static Node*  pForStatement         (ParserState*);
static Node*  pAssignmentExpression (ParserState*);
static Node*  pExpression           (ParserState*);
static Node*  pTerm                 (ParserState*);
static Node*  pFactor               (ParserState*);
static Node*  pFunctionCall         (ParserState*);
static Node*  pExpressionList       (ParserState*);

static bool   accept        (ParserState* p, int type);
static bool   expect        (ParserState* p, int type);
static bool   isExpression  (ParserState* p);
static void   node_set_token(ParserState*, Node*);

static Node   NodeEnd = {0};
static Node*  pEnd    = &NodeEnd;


#define SETERROR(format, ...) \
  errstate_adderror(&p->error, "[%d:%d] " #format, p->lexer.current_line, p->lexer.current_column, __VA_ARGS__)
#define TOKEN()     (p->lexer.current_token)
#define PEEK()      (p->lexer.token)
#define ACCEPT(t)   ( accept(p, (t)) )
#define EXPECT(t)   { if( expect(p, (t)) ) goto error; }
#define NEXT()      { lexer_next(&p->lexer); }
#define ASSERT(x)   { if( !(x) ) goto error; }

int parser_initialize(ParserState* p, const char* filename)
{
  errstate_initialize(&p->error);
  p->root = NULL;

  if( !lexer_initialize(&p->lexer, filename) )
    return ERROR;

  return SUCCESS;
}

int parser_terminate(ParserState* p)
{
  lexer_terminate(&p->lexer);
  errstate_terminate(&p->error);
  node_delete_tree(p->root);

  return SUCCESS;
}

int parser_generate(ParserState* p)
{
  if( p->root ) return ERROR;

  p->root = pStatements(p);
  if( !p->root ) return ERROR;

  return SUCCESS;
}

bool accept(ParserState* p, int type)
{
  if( type==TOKEN() )
  {
    lexer_next(&p->lexer);
    return true;
  }

  return false;
}

bool expect(ParserState* p, int type)
{
  if( !accept(p, type) )
  {
    SETERROR("Expected %d token", type);
    return false;
  }

  return true;
}

Node* pStatements(ParserState* p)
{
  Node* parent = NULL;

  while( true )
  {
    Node* statement = pStatement(p);
    if( !statement ) 
      break;
    if( statement==pEnd )
      return parent;

    parent = parent ? node_alloc2(N_GLUE, parent, statement) : statement ;
  }

  // Error
  node_delete_tree(parent);
  return NULL;
}

Node* pStatement(ParserState* p)
{
  if( ACCEPT(T_IF) )
    return pIfStatement(p);
  else if( ACCEPT(T_FUNCTION) )
    return pFunction(p);
  else if( ACCEPT(T_RETURN) )
    return pReturnStatement(p);
  else if( ACCEPT(T_VAR) )
    return pVariableDefinition(p);
  else if( ACCEPT(T_FOR) )
    return pForStatement(p);
  else if( ACCEPT(T_LBRACE) )
    return pBlockStatement(p);
  else if( isExpression(p) )
    return pExpressionStatement(p);
  
  SETERROR("Unexpected input");
  return NULL;
}

Node* pForStatement(ParserState* p)
{
  Node* setup = NULL;
  Node* cond  = NULL;
  Node* each  = NULL;
  Node* body  = NULL;

  setup = pAssignmentExpression(p);
  ASSERT(setup);
  EXPECT(T_SEMICOLON);

  cond = pAssignmentExpression(p);
  ASSERT(cond);
  EXPECT(T_SEMICOLON);

  each = pAssignmentExpression(p);
  ASSERT(each);
  EXPECT(T_RPREN);

  body = pStatement(p);
  return node_alloc2(N_BLOCK, setup, node_alloc3(N_FOR, cond, each, body));

error:
  node_delete_tree(setup);
  node_delete_tree(cond);
  node_delete_tree(each);
  node_delete_tree(body);
  return NULL;
}

Node* pVariableDefinition(ParserState* p)
{
  Node* name = NULL;
  Node* expr = NULL;


  if( TOKEN()!=T_IDENT )
  {
    SETERROR("Expected variable name");
    goto error;
  }
  node_set_token(p, name);
  NEXT();

  if( ACCEPT(T_ASSIGNMENT) )
  {
    expr = pExpression(p);
    ASSERT(expr);
  }

  EXPECT(T_SEMICOLON);
  return node_alloc2(N_VAR, name, expr);

error:
  node_delete_tree(name);
  node_delete_tree(expr);
  return NULL;
}

Node* pReturnStatement(ParserState* p)
{
  Node* expr = NULL;

  if( isExpression(p) )
  {
    expr = pAssignmentExpression(p);
    ASSERT(expr);
    EXPECT(T_SEMICOLON);
  }
  return expr;

error:
  node_delete_tree(expr);
  return NULL;
}

Node* pBlockStatement(ParserState* p)
{
  Node* root = NULL;

  root = pStatements(p);
  ASSERT(root);
  EXPECT(T_RBRACE);
  return root;

error:
  node_delete_tree(root);
  return NULL;
}

Node* pFunction(ParserState* p)
{
  Node* name    = NULL;
  Node* params  = NULL;
  Node* body    = NULL;


  if( TOKEN()==T_IDENT )
  {
    name = node_alloc0(N_IDENT);
    node_set_token(p, name);
    NEXT();
  }
  else
  {
    // I might be able to get away with just NULL
    name = node_alloc0(N_ANONYMOUS);
  }

  EXPECT(T_LPREN);
  params = pParameterDefinition(p);
  ASSERT(params);
  
  EXPECT(T_LBRACE);
  body = pBlockStatement(p);
  ASSERT(body);
 
  return node_alloc3(N_FUNCTION, name, params, body);

error:
  node_delete_tree(name);
  node_delete_tree(params);
  node_delete_tree(body);
  return NULL;
}

Node* pParameterDefinition(ParserState* p)
{
  Node* root = NULL;

  while( true )
  {
    if( TOKEN()==T_IDENT )
    {
      Node* ident = node_alloc0(N_IDENT);
      node_set_token(p, ident);
      root = root ? node_alloc2(N_GLUE, root, ident) : ident ;
      NEXT();
      ACCEPT(T_COMMA);
    }
    else if( ACCEPT(T_RPREN) )
    {
      break;
    }
    else
    {
      SETERROR("Syntax error in parameter definition");
      goto error;
    }
  }

  return root;

error:
  node_delete_tree(root);
  return NULL;
}

Node* pIfStatement(ParserState* p)
{
  Node* condition = NULL;
  Node* onTrue    = NULL;
  Node* onFalse   = NULL;

  
  EXPECT(T_LPREN);
  condition = pExpression(p);
  ASSERT(condition);
  EXPECT(T_RPREN);

  onTrue = pStatement(p);
  ASSERT(onTrue);

  if( ACCEPT(T_ELSE) )
  {
    onFalse = pStatement(p);
    ASSERT(onFalse);
  }
  
  return node_alloc3(N_IF, condition, onTrue, onFalse);

error:
  node_delete_tree(condition);
  node_delete_tree(onTrue);
  node_delete_tree(onFalse);
  return NULL;
}


Node* pExpressionStatement(ParserState* p)
{
  Node* expr = NULL;

  expr = pAssignmentExpression(p);
  ASSERT(expr);
  EXPECT(T_SEMICOLON);
  return expr;

error:
  node_delete_tree(expr);
  return NULL;
}

Node* pAssignmentExpression(ParserState* p)
{
  Node* lparam = NULL;
  Node* rparam = NULL;

  lparam = pExpression(p);
  ASSERT(lparam);

  if( TOKEN()!=T_ASSIGNMENT ) 
    return lparam;

  NEXT();
  rparam = pExpression(p);
  ASSERT(rparam);

  return node_alloc2(N_ASSIGNMENT, lparam, rparam);

error:
  node_delete_tree(lparam);
  node_delete_tree(rparam);
  return NULL;
}

Node* pExpression(ParserState* p)
{
  Node* root = pTerm(p);
  ASSERT(root);


  while( true )
  {
    if( TOKEN()==T_PLUS )
    {
      Node* term = NULL;

      NEXT();
      term = pTerm(p);
      ASSERT(term);
      
      root = node_alloc2(N_ADD, root, term);
    }
    else if( TOKEN()==T_MINUS )
    {
      Node* term = NULL;

      NEXT();
      term = pTerm(p);
      ASSERT(term);
      
      root = node_alloc2(N_SUB, root, term);
    }
    else
    {
      break;
    }
  }

error:
  node_delete_tree(root);
  return NULL;
}

Node* pTerm(ParserState* p)
{
  Node* root = pFactor(p);
  ASSERT(root);


  while( true )
  {
    if( TOKEN()==T_PLUS )
    {
      Node* factor = NULL;

      NEXT();
      factor = pFactor(p);
      ASSERT(factor);
      
      root = node_alloc2(N_ADD, root, factor);
    }
    else if( TOKEN()==T_MINUS )
    {
      Node* factor = NULL;

      NEXT();
      factor = pFactor(p);
      ASSERT(factor);
      
      root = node_alloc2(N_SUB, root, factor);
    }
    else
    {
      break;
    }
  }

error:
  node_delete_tree(root);
  return NULL;
}

Node* pFactor(ParserState* p)
{
  Node* n = NULL;


  switch( TOKEN() )
  {
    case T_NUMBER:
    case T_BIN_NUMBER:
    case T_HEX_NUMBER:
    case T_OCT_NUMBER:
    {
      n = node_alloc0(N_N_LITERAL);
      node_set_token(p, n);
      NEXT();
      break;
    }

    case T_IDENT:
    {
      if( PEEK()==T_LPREN )
      {
        //
        // Function call
        //
        n = pFunctionCall(p);
        ASSERT(n);
      }
      else
      {
        //
        // Variable
        //
        n = node_alloc0(N_IDENT);
        node_set_token(p, n);
        NEXT();
      }
      break;
    }

    case T_LPREN:
    {
      NEXT();
      n = pAssignmentExpression(p);
      ASSERT(n);
      EXPECT(T_RPREN);

      break;
    }

    default:
      SETERROR("Unexpected input");
      goto error;
  }

  return n;

error:
  node_delete_tree(n);
  return NULL;
}

Node* pFunctionCall(ParserState* p)
{
  Node* name    = NULL;
  Node* params  = NULL;


  name = node_alloc0(N_IDENT);
  node_set_token(p, name);
  NEXT();

  EXPECT(T_LPREN);
  params = pExpressionList(p);
  ASSERT(params);
  EXPECT(T_RPREN);

  return node_alloc2(N_CALL, name, params);

error:
  node_delete_tree(name);
  node_delete_tree(params);
  return NULL;
}

Node* pExpressionList(ParserState* p)
{
  Node* list = NULL;

  while( true )
  {
    if( TOKEN()==T_RPREN )
    {
      break;
    }
    else if( isExpression(p) )
    {
      Node* expr = pAssignmentExpression(p);
      ASSERT(expr);
      list = list ? node_alloc2(N_NEXT, list, expr) : expr ;
      ACCEPT(T_COMMA);
    }
  }

 error:
  node_delete_tree(list);
  return NULL;
}

void node_set_token(ParserState* p, Node* n)
{
  n->token_type = p->lexer.current_token;
  n->line       = p->lexer.current_line;
  n->column     = p->lexer.current_column;
  string_copy(&p->lexer.current_raw, &n->raw);
}

bool isExpression(ParserState* p)
{
  switch( TOKEN() )
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