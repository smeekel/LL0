#include <stdio.h>
#include "irgenerator.h"


static void printOp(IROp*);


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
