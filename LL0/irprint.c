#include <stdio.h>
#include "irgenerator.h"


static void printFunction (const Function*);
static void printOp       (const IROp*);
static void printSymbol   (const Symbol*);
static void printImport   (const Import*);


void irgen_print(IRGenerator* p)
{
  printf(".MODULE\n");

  for( LListNode* n=p->module.imports.set.first ; n ; n=n->next )
    printImport((Import*)n);

  for( LListNode* n=p->module.symtab.symbols.first ; n ; n=n->next )
    printSymbol((Symbol*)n);

  printf("\n");

  for( LListNode* n=p->module.functions.first ; n ; n=n->next )
    printFunction((Function*)n);


  printf(".MODULE-END\n");
}

void printImport(const Import* p)
{
  if( string_is_empty(&p->as) )
    printf("  .imp   %s\n", p->name.buffer);
  else
    printf("  .imp   %s -> %s\n", p->name.buffer, p->as.buffer);
}

void printFunction(const Function* fn)
{
  printf
  (
    "\n%s:\n", 
    string_is_empty(&fn->name)
    ? "_entry"
    : fn->name.buffer
  );

  for( LListNode* n=fn->ops.first ; n ; n=n->next )
    printOp((IROp*)n);

}

void printOp(const IROp* op)
{
  switch( op->op )
  {
    case NOP:     printf("  NOP\n"); break;
    case PUSH:    printf("  PUSH    $%i\n", op->A); break;
    case DISCARD: printf("  DISCARD %i\n",  op->A); break;
    case MOV:     printf("  MOV     $%i, $%i\n", op->A, op->B); break;
    case SMOV:    printf("  SMOV    %i, $%i\n", op->A, op->B); break;
    case LOADK:   printf("  LOADK   '%s', $%i\n", op->raw.buffer, op->B); break;
    case CALL:    printf("  CALL    @L%i, %i\n", op->A, op->B); break;
    case JMP:     printf("  JMP     @L%i\n", op->A); break;
    case JMPF:    printf("  JMPF    @L%i <$%i>\n", op->A, op->B);

    case GET:     printf("  GET     $%i, $%i, $%i\n", op->A, op->B, op->C); break;
    case ADD:     printf("  ADD     $%i, $%i, $%i\n", op->A, op->B, op->C); break;
    case SUB:     printf("  SUB     $%i, $%i, $%i\n", op->A, op->B, op->C); break;
    case MUL:     printf("  MUL     $%i, $%i, $%i\n", op->A, op->B, op->C); break;
    case DIV:     printf("  DIV     $%i, $%i, $%i\n", op->A, op->B, op->C); break;

    case RET:
      if( op->A ) printf("  RET     %i\n", op->A); 
      else        printf("  RET\n");
      break;

    case LABEL:   printf("L%i:\n", op->A); break;

    default:
      printf("  (%02X)\n", op->op);
      break;
  }
}

void printSymbol(const Symbol* s)
{
  printf
  (
    "  .sym %c%c[%s]:%i\n",
    s->flags&F_PUBLIC   ? 'P' : ' ',
    s->flags&F_FUNCTION ? 'F' : ' ',
    s->name.buffer,
    s->vindex
  );
}
