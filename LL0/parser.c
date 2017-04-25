#include "parser.h"
#include "tokens.h"


static Node*  pStatements           (Parser*);
static Node*  pStatement            (Parser*);
static Node*  pExpressionStatement  (Parser*);
static Node*  pBlockStatement       (Parser*);
static Node*  pReturnStatement      (Parser*);
static Node*  pVariableDefinition   (Parser*);
static Node*  pFunction             (Parser*);
static Node*  pParameterDefinition  (Parser*);
static Node*  pIfStatement          (Parser*);
static Node*  pForStatement         (Parser*);
static Node*  pAssignmentExpression (Parser*);
static Node*  pExpression           (Parser*);
static Node*  pTerm                 (Parser*);
static Node*  pFactor               (Parser*);
static Node*  pFunctionCall         (Parser*);
static Node*  pExpressionList       (Parser*);

static bool   accept        (Parser* p, int type);
static bool   expect        (Parser* p, int type);
static bool   isExpression  (Parser* p);
static void   node_set_token(Parser*, Node*);

static Node   NodeEnd = {0};
static Node*  pEnd    = &NodeEnd;


#define SETERROR(format, ...) \
  errstate_adderror(&p->error, "[%d:%d] " #format, p->lexer.current_line, p->lexer.current_column, __VA_ARGS__)
#define TOKEN()     (p->lexer.current_token)
#define PEEK()      (p->lexer.token)
#define ACCEPT(t)   ( accept(p, (t)) )
#define EXPECT(t)   { if( !expect(p, (t)) ) goto error; }
#define NEXT()      { lexer_next(&p->lexer); }
#define ASSERT(x)   { if( !(x) ) goto error; }


int parser_initialize(Parser* p, const char* filename)
{
  errstate_initialize(&p->error);
  p->root = NULL;

  if( !lexer_initialize(&p->lexer, filename) )
    return ERROR;
  lexer_next(&p->lexer);

  return SUCCESS;
}

int parser_terminate(Parser* p)
{
  lexer_terminate(&p->lexer);
  errstate_terminate(&p->error);
  node_delete_tree(p->root);

  return SUCCESS;
}

int parser_generate(Parser* p)
{
  if( p->root ) return ERROR;

  p->root = pStatements(p);
  if( !p->root ) return ERROR;
  node_print_tree(p->root);

  return SUCCESS;
}

bool accept(Parser* p, int type)
{
  if( type==TOKEN() )
  {
    lexer_next(&p->lexer);
    return true;
  }

  return false;
}

bool expect(Parser* p, int type)
{
  if( !accept(p, type) )
  {
    SETERROR("Expected %d token", type);
    return false;
  }

  return true;
}

Node* pStatements(Parser* p)
{
  Node* parent = NULL;

  while( true )
  {
    Node* statement = pStatement(p);
    ASSERT(statement);
    if( statement==pEnd ) break;

    parent = parent ? node_alloc2(N_GLUE, parent, statement) : statement ;
  }
  return parent;

error:
  // Error
  node_delete_tree(parent);
  return NULL;
}

Node* pStatement(Parser* p)
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

  return pEnd;
}

Node* pForStatement(Parser* p)
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

Node* pVariableDefinition(Parser* p)
{
  Node* name = NULL;
  Node* expr = NULL;


  if( TOKEN()!=T_IDENT )
  {
    SETERROR("Expected variable name");
    goto error;
  }
  name = node_alloc0(N_IDENT);
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

Node* pReturnStatement(Parser* p)
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

Node* pBlockStatement(Parser* p)
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

Node* pFunction(Parser* p)
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

Node* pParameterDefinition(Parser* p)
{
  Node* root = NULL;

  while( true )
  {
    if( TOKEN()==T_IDENT )
    {
      Node* ident = node_alloc0(N_IDENT);
      node_set_token(p, ident);
      root = root ? node_alloc2(N_NEXT, root, ident) : ident ;
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

Node* pIfStatement(Parser* p)
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


Node* pExpressionStatement(Parser* p)
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

Node* pAssignmentExpression(Parser* p)
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

Node* pExpression(Parser* p)
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
  return root;

error:
  node_delete_tree(root);
  return NULL;
}

Node* pTerm(Parser* p)
{
  Node* root = pFactor(p);
  ASSERT(root);


  while( true )
  {
    if( TOKEN()==T_MUL )
    {
      Node* factor = NULL;

      NEXT();
      factor = pFactor(p);
      ASSERT(factor);
      
      root = node_alloc2(N_MUL, root, factor);
    }
    else if( TOKEN()==T_DIV )
    {
      Node* factor = NULL;

      NEXT();
      factor = pFactor(p);
      ASSERT(factor);
      
      root = node_alloc2(N_DIV, root, factor);
    }
    else
    {
      break;
    }
  }
  return root;

error:
  node_delete_tree(root);
  return NULL;
}

Node* pFactor(Parser* p)
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

Node* pFunctionCall(Parser* p)
{
  Node* name    = NULL;
  Node* params  = NULL;


  name = node_alloc0(N_IDENT);
  node_set_token(p, name);
  NEXT();

  EXPECT(T_LPREN);
  params = pExpressionList(p);
  ASSERT(params);

  return node_alloc2(N_CALL, name, params);

error:
  node_delete_tree(name);
  node_delete_tree(params);
  return NULL;
}

Node* pExpressionList(Parser* p)
{
  Node* list = NULL;

  while( true )
  {
    if( ACCEPT(T_RPREN) )
      break;

    if( isExpression(p) )
    {
      Node* expr = pAssignmentExpression(p);
      ASSERT(expr);
      list = list ? node_alloc2(N_NEXT, list, expr) : expr ;
      ACCEPT(T_COMMA);
    }
  }

  return list;

 error:
  node_delete_tree(list);
  return NULL;
}

void node_set_token(Parser* p, Node* n)
{
  n->token_type = p->lexer.current_token;
  n->line       = p->lexer.current_line;
  n->column     = p->lexer.current_column;
  string_copy(&p->lexer.current_raw, &n->raw);
}

bool isExpression(Parser* p)
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