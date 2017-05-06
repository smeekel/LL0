#include <malloc.h>
#include <stdio.h>
#include "irgenerator.h"
#include "node.h"


static int    gBlock                  (IRGenerator*, const Node*);
static int    gVar                    (IRGenerator*, const Node*);
static int    gIf                     (IRGenerator*, const Node*);
static int    gCall                   (IRGenerator*, const Node*);
static int    gPushCallParameters     (IRGenerator*, const Node*);
static int    gFunction               (IRGenerator*, const Node*);
static int    gFunctionPushParameters (IRGenerator* p, const Node* n, int* index);
static int    gReturn                 (IRGenerator*, const Node*);
static int    gEval                   (IRGenerator*, const Node*);
static int    gOperator2              (IRGenerator*, const Node*);
static int    gLiteral                (IRGenerator*, const Node*);
static int    gIdent                  (IRGenerator*, const Node*);
static int    gAssignment             (IRGenerator*, const Node*);

static IROp*  op0             (IRGenerator*, const int type);
static IROp*  op1             (IRGenerator*, const int type, const int A);
static IROp*  op2             (IRGenerator*, const int type, const int A, const int B);
static IROp*  op3             (IRGenerator*, const int type, const int A, const int B, const int C);
static void   label           (IRGenerator*, const int id);
static void   deleteOp        (IROp*);
static int    countParameters (const Node*);
static int    patchRegisters  (IRGenerator*);


#define EXCEPTION(n, format, ...) {\
  errstate_adderror(&p->errors, "[%d:%d] " #format, n->line, n->column, __VA_ARGS__); \
  goto error; \
}

#define ASSERT( x )   { if( (x)==ERROR ) goto error; }
#define NEW_LABEL(p)  ( ++(p)->labelIndex )


int irgen_initialize(IRGenerator* p)
{
  errstate_initialize(&p->errors);
  symtab_initialize(&p->symtab);
  llist_initialize(&p->ops, (llistNodeDelete)deleteOp);
  p->labelIndex = 0;
  return SUCCESS;
}

int irgen_terminate(IRGenerator* p)
{
  llist_terminate(&p->ops);
  symtab_terminate(&p->symtab);
  errstate_terminate(&p->errors);
  return SUCCESS;
}

int irgen_generate(IRGenerator* p, Parser* parser)
{
  if( !gBlock(p, parser->root) )
    return ERROR;
  if( !patchRegisters(p) )
    return ERROR;

  return SUCCESS;
}

int patchRegisters(IRGenerator* p)
{
  //const int localCount = p->

  return SUCCESS;
}

static void deleteOp(IROp* op)
{
  string_terminate(&op->raw);
  free(op);
}

IROp* op0(IRGenerator* p, const int type)
{
  IROp* op = (IROp*)malloc(sizeof(IROp));
  string_initialize(&op->raw);
  llist_node_initialize((LListNode*)op);

  op->op      = type;
  op->A       = 0;
  op->B       = 0;
  op->C       = 0;
  op->linkup  = NULL;
  llist_add_end(&p->ops, (LListNode*)op);

  return op;
}

IROp* op1(IRGenerator* p, const int type, const int A)
{
  IROp* op = (IROp*)malloc(sizeof(IROp));
  string_initialize(&op->raw);
  llist_node_initialize((LListNode*)op);

  op->op      = type;
  op->A       = A;
  op->B       = 0;
  op->C       = 0;
  op->linkup  = NULL;
  llist_add_end(&p->ops, (LListNode*)op);

  return op;
}

IROp* op2(IRGenerator* p, const int type, const int A, const int B)
{
  IROp* op = (IROp*)malloc(sizeof(IROp));
  string_initialize(&op->raw);
  llist_node_initialize((LListNode*)op);

  op->op      = type;
  op->A       = A;
  op->B       = B;
  op->C       = 0;
  op->linkup  = NULL;
  llist_add_end(&p->ops, (LListNode*)op);

  return op;
}

IROp* op3(IRGenerator* p, const int type, const int A, const int B, const int C)
{
  IROp* op = (IROp*)malloc(sizeof(IROp));
  string_initialize(&op->raw);
  llist_node_initialize((LListNode*)op);

  op->op      = type;
  op->A       = A;
  op->B       = B;
  op->C       = C;
  op->linkup  = NULL;
  llist_add_end(&p->ops, (LListNode*)op);

  return op;
}

void label(IRGenerator* p, const int id)
{
  op1(p, LABEL, id);
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
    ASSERT( temp );
    op2(p, MOV, temp, sym->vindex);
  }

  return SUCCESS;

error:
  return ERROR;
}

int gIf(IRGenerator* p, const Node* n)
{
  const Node* cond    = n->A;
  const Node* onTrue  = n->B;
  const Node* onFalse = n->C;

  const int condTemp    = gEval(p, cond);
  const int labelFalse  = NEW_LABEL(p);

  op2(p, JMPF, labelFalse, condTemp);
  gBlock(p, onTrue);

  if( !onFalse )
  {
    label(p, labelFalse);
  }
  else
  {
    const int labelExit = NEW_LABEL(p);
    op1   (p, JMP, labelExit);
    label (p, labelFalse);
    gBlock(p, onFalse);
    label (p, labelExit);
  }

  return SUCCESS;
}

int gFunction(IRGenerator* p, const Node* n)
{
  const Node* ident   = n->A;
  const Node* params  = n->B;
  const Node* body    = n->C;

  Symbol* function = symtab_new(&p->symtab, ident->raw.buffer);
  if( !function )
    EXCEPTION(n, "Duplicate variable name [%s]", ident->raw.buffer);
  function->flags   = F_FUNCTION;
  function->vindex  = NEW_LABEL(p);

  symtab_scope_push(&p->symtab);
  label(p, function->vindex);

  int index = 0;
  ASSERT( gFunctionPushParameters(p, params, &index) );
  gBlock(p, body);
  op0(p, RET);

  symtab_scope_pop(&p->symtab);

  return SUCCESS;

error:
  return ERROR;
}

int gFunctionPushParameters(IRGenerator* p, const Node* n, int* index)
{
  if( n->type==N_NEXT )
  {
    ASSERT( gFunctionPushParameters(p, n->A, index) );
    ASSERT( gFunctionPushParameters(p, n->B, index) );
  }
  else if( n->type==N_IDENT )
  {
    Symbol* sym = symtab_new(&p->symtab, n->raw.buffer);
    if( !sym )
      EXCEPTION(n, "Duplicate variable name [%s]", n->raw.buffer);
    op2(p, SMOV, (*index)++, sym->vindex);
  }
  else
  {
    EXCEPTION(n, "Unsupported parameter type");
  }

  return SUCCESS;

error:
  return ERROR;
}

int gCall(IRGenerator* p, const Node* n)
{
  const Node* ident = n->A;
  const int   temp  = symtab_new_vindex(&p->symtab);

  if( ident->type!=N_IDENT )
    EXCEPTION(n, "(ICE) N_CALL.A != N_IDENT");

  Symbol* symbol = symtab_find(&p->symtab, ident->raw.buffer);
  if( !symbol )
    EXCEPTION(ident, "Undefined function [%s]", ident->raw.buffer);
  if( !(symbol->flags&F_FUNCTION) )
    EXCEPTION(ident, "Symbol [%s] is not a funciton", ident->raw.buffer);

  const int count = countParameters(n->B);
  ASSERT( gPushCallParameters(p, n->B) );

  op2(p, CALL, symbol->vindex, count);
  op2(p, SMOV, 0, temp);
  op1(p, DISCARD, count+1);

  return temp;

error:
  return ERROR;
}

int gPushCallParameters(IRGenerator* p, const Node* n)
{
  if( n->type==N_NEXT )
  {
    gPushCallParameters(p, n->A);
    gPushCallParameters(p, n->B);
  }
  else
  {
    const int temp = gEval(p, n);
    ASSERT( temp );
    op1(p, PUSH, temp);
  }

  return SUCCESS;

error:
  return ERROR;
}

int gReturn(IRGenerator* p, const Node* n)
{
  const Node* expr = n->A;

  if( expr )
  {
    const int temp = gEval(p, expr);
    op1(p, PUSH, temp);
    op1(p, RET, 1);
  }
  else
  {
    op0(p, RET);
  }

  return SUCCESS;
}

int gEval(IRGenerator* p, const Node* n)
{
  int temp;

  switch( n->type )
  {
    case N_ADD: 
    case N_SUB:
    case N_MUL:
    case N_DIV: 
      ASSERT( temp = gOperator2(p, n) ); 
      break;

    case N_N_LITERAL: 
      ASSERT( temp = gLiteral(p, n) )
      break;

    case N_IDENT:
      ASSERT( temp = gIdent(p, n) );
      break;

    case N_CALL:
      ASSERT( temp = gCall(p, n) );
      break;

    case N_ASSIGNMENT:
      ASSERT( temp = gAssignment(p, n) );
      break;

    default:
      printf("> gEval:Unknown %i\n", n->type);
      break;
  }

  return temp;

error:
  return ERROR;
}

int gOperator2(IRGenerator* p, const Node* n)
{
  const int temp  = symtab_new_vindex(&p->symtab);
  const int a     = gEval(p, n->A);
  const int b     = gEval(p, n->B);
  int op;

  switch( n->type )
  {
    case N_ADD: op = ADD; break;
    case N_SUB: op = SUB; break;
    case N_MUL: op = MUL; break;
    case N_DIV: op = DIV; break;
    default:
      EXCEPTION(n, "Internal error: gOperator2 call with unsupported operator %i\n", n->type);
  }

  op3(p, op, a, b, temp);

  return temp;

error:
  return ERROR;
}

int gLiteral(IRGenerator* p, const Node* n)
{
  const int temp = symtab_new_vindex(&p->symtab);

  IROp* op = op2(p, LOADK, 0, temp);
  string_copy(&n->raw, &op->raw);
  return temp;
}

int gAssignment(IRGenerator* p, const Node* n)
{
  const Node* lparam  = n->A;
  const Node* rparam  = n->B;

  if( lparam->type!=N_IDENT )
    EXCEPTION(n, "lparam must be an identifier");

  const Symbol* sym = symtab_find(&p->symtab, lparam->raw.buffer);
  if( !sym )
    EXCEPTION(n, "Undefined variable [%s]", lparam->raw.buffer);

  const int tempExpr = gEval(p, rparam);
  op2(p, MOV, tempExpr, sym->vindex);

  return sym->vindex;

error:
  return ERROR;
}

int gIdent(IRGenerator* p, const Node* n)
{
  const Symbol* sym = symtab_find(&p->symtab, n->raw.buffer);

  if( !sym )
    EXCEPTION(n, "Undefined variable [%s]", n->raw.buffer);

  return sym->vindex;

error:
  return ERROR;
}

int countParameters(const Node* n)
{
  if( n->type==N_NEXT )
  {
    return countParameters(n->A) + countParameters(n->B);
  }
  else
  {
    return 1;
  }
}
