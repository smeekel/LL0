#include <malloc.h>
#include <stdio.h>
#include "irgenerator.h"
#include "node.h"


static int    gBlock      (IRGenerator*, const Node*);
static int    gVar        (IRGenerator*, const Node*);
static int    gIf         (IRGenerator*, const Node*);
static int    gCall       (IRGenerator*, const Node*);
static int    gFunction   (IRGenerator*, const Node*);
static int    gReturn     (IRGenerator*, const Node*);
static int    gEval       (IRGenerator*, const Node*);
static int    gOperator2  (IRGenerator*, const Node*);
static int    gLiteral    (IRGenerator*, const Node*);
static int    gIdent      (IRGenerator*, const Node*);
static int    gAssignment (IRGenerator*, const Node*);

static IROp*  op1         (IRGenerator*, const int type, const int A);
static IROp*  op2         (IRGenerator*, const int type, const int A, const int B);
static IROp*  op3         (IRGenerator*, const int type, const int A, const int B, const int C);
static void   label       (IRGenerator*, const int id);
static void   deleteOp    (IROp*);
static void   printOp     (IROp*);


#define EXCEPTION(n, format, ...) {\
  errstate_adderror(&p->errors, "[%d:%d] " #format, n->line, n->column, __VA_ARGS__); \
  goto error; \
}

#define ASSERT( x )  { if( (x)==ERROR ) goto error; }


int irgen_initialize(IRGenerator* p)
{
  errstate_initialize(&p->errors);
  symtab_initialize(&p->symtab);
  llist_initialize(&p->ops, (llistNodeDelete)deleteOp);
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
  return SUCCESS;
}

void irgen_print(IRGenerator* p)
{
  printf(".MODULE\n");
  for( LListNode* n=p->ops.first ; n ; n=n->next )
  {
    printOp((IROp*)n);
  }
  printf(".MODULE-END\n");
}

void printOp(IROp* op)
{
  switch( op->op )
  {
    case NOP:     printf("  NOP\n"); break;
    case PUSH:    printf("  PUSH    $%i\n", op->A); break;
    case POP:     printf("  POP     #%i\n", op->A); break;
    case MOV:     printf("  MOV     $%i, $%i\n", op->A, op->B); break;
    case SMOV:    printf("  SMOV    #%i, $%i\n", op->A, op->B); break;
    case LOADK:   printf("  LOADK   L%s, $%i\n", op->raw.buffer, op->B); break;
    case CALL:    printf("  CALL    $%i, #%i\n", op->A, op->B); break;
    case JMP:     printf("  JMP     @%i\n", op->A); break;
    case JMPF:    printf("  JMPF    @%i <$%i>\n", op->A, op->B);
    
    case ADD:     printf("  ADD     $%i, $%i, $%i\n", op->A, op->B, op->C); break;
    case SUB:     printf("  SUB     $%i, $%i, $%i\n", op->A, op->B, op->C); break;
    case MUL:     printf("  MUL     $%i, $%i, $%i\n", op->A, op->B, op->C); break;
    case DIV:     printf("  DIV     $%i, $%i, $%i\n", op->A, op->B, op->C); break;

    case RET:
      if( op->A ) printf("  RET     $%i\n", op->A); 
      else        printf("  RET\n");
      break;

    case LABEL:   printf("%i:\n", op->A); break;

    default:
      printf("  (%02X)\n", op->op);
      break;
  }
}

static void deleteOp(IROp* op)
{
  string_terminate(&op->raw);
  free(op);
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
  const int labelFalse  = symtab_new_vindex(&p->symtab);

  op2(p, JMPF, labelFalse, condTemp);
  gBlock(p, onTrue);

  if( !onFalse )
  {
    label(p, condTemp);
  }
  else
  {
    const int labelExit = symtab_new_vindex(&p->symtab);
    op1   (p, JMP, labelExit);
    label (p, condTemp);
    gBlock(p, onFalse);
    label (p, labelExit);
  }

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
