#include "irgenerator.h"
#include "node.h"


static int    gBlock      (IRGenerator*, const Node*);
static int    gVar        (IRGenerator*, const Node*);
static int    gIf         (IRGenerator*, const Node*);
static int    gCall       (IRGenerator*, const Node*);
static int    gFunction   (IRGenerator*, const Node*);
static int    gReturn     (IRGenerator*, const Node*);
static int    gEval       (IRGenerator*, const Node*);

static IROp*  op2         (IRGenerator*, const int type, const int A, const int B);


#define EXCEPTION(n, format, ...) {\
  errstate_adderror(&p->errors, "[%d:%d] " #format, n->line, n->column, __VA_ARGS__); \
  goto error; \
}

#define ASSERT( x )  { if( !(x) ) goto error; }


int irgen_initialize(IRGenerator* p)
{
  errstate_initialize(&p->errors);
  symtab_initialize(&p->symtab);
  return SUCCESS;
}

int irgen_terminate(IRGenerator* p)
{
  symtab_terminate(&p->symtab);
  errstate_terminate(&p->errors);
  return SUCCESS;
}

int irgen_generate(IRGenerator* p, Parser* parser)
{
  if( !gBlock(p, parser->root) )
    return ERROR;
  return SUCCESS;
}

static int gBlock(IRGenerator* p, const Node* n)
{
  switch( n->type )
  {
    case N_GLUE:
      ASSERT( gBlock(p, n->A) );
      ASSERT( gBlock(p, n->B) );
      break;

    case N_VAR:       ASSERT( gVar(p, n)      ) break;
    case N_CALL:      ASSERT( gCall(p, n)     ) break;
    case N_IF:        ASSERT( gIf(p, n)       ) break;
    case N_FUNCTION:  ASSERT( gFunction(p, n) ) break;
    case N_RETURN:    ASSERT( gReturn(p, n)   ) break;
    default:          ASSERT( gEval(p, n)     ) break;
  }

  return SUCCESS;

error:
  return ERROR;
}

int gVar(IRGenerator* p, const Node* n)
{
  const Node* ident = n->A;
  const Node* expr  = n->B;
  Symbol* sym = symtab_new(&p->symtab, ident->raw.buffer);

  if( !sym ) 
    EXCEPTION(ident, "Duplicate identifier name in current score [%s]", ident->raw.buffer);

  if( expr )
  {
    const int temp = gEval(p, expr);
    op2(p, MOV, temp, sym->vindex);
  }

  return SUCCESS;

error:
  return ERROR;
}

int gIf(IRGenerator* p, const Node* n)
{
  return SUCCESS;
}

int gFunction(IRGenerator* p, const Node* n)
{
  return SUCCESS;
}

int gCall(IRGenerator* p, const Node* n)
{
  return SUCCESS;
}

int gReturn(IRGenerator* p, const Node* n)
{
  return SUCCESS;
}

int gEval(IRGenerator* p, const Node* n)
{
  return SUCCESS;
}


IROp* op2(IRGenerator* p, const int type, const int A, const int B)
{
  return NULL;  
}

