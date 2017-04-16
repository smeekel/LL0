#include "IRModule.h"

using namespace LL0;


IRModule::IRModule()
{
}

int IRModule::newVirtualIndex()
{
  return sym.newVirtualIndex();
}

void IRModule::addOp(IROp op)
{
  ops.push_back(op);
}

void IRModule::addOp(const IROps op)
{
  ops.push_back(IROp(op));
}

void IRModule::addOp(const IROps op, const int a)
{
  IROp ir(op);
  ir.A = a;
  ops.push_back(ir);
}

void IRModule::addOp(const IROps op, const int a, const int b)
{
  IROp ir(op);
  ir.A = a;
  ir.B = b;
  ops.push_back(ir);
}

void IRModule::addOp(const IROps op, const int a, const int b, const int c)
{
  IROp ir(op);
  ir.A = a;
  ir.B = b;
  ir.C = c;
  ops.push_back(ir);
}

void IRModule::print()
{
  printf(".MODULE\n");

  for( auto i=ops.begin() ; i!=ops.end() ; i++ )
    print_op(*i);

  printf(".ENDMODULE\n");
}

void IRModule::print_op(const IROp& op)
{
  switch( op.op )
  {
    case PUSH:    printf("  PUSH    $%i\n", op.A); break;
    case POP:     printf("  POP     #%i\n", op.A); break;
    case MOV:     printf("  MOV     $%i, $%i\n", op.A, op.B); break;
    case SMOV:    printf("  SMOV    #%i, $%i\n", op.A, op.B); break;
    case LOADK:   printf("  LOADK   L%s, $%i\n", op.raw.c_str(), op.B); break;
    case CALL:    printf("  CALL    $%i, #%i\n", op.A, op.B); break;
    case JMP:     printf("  JMP     $%i\n", op.A); break;
    case JMPF:    printf("  JMPF    $%i <$%i>\n", op.A, op.B);
    
    case ADD:     printf("  ADD     $%i, $%i, $%i\n", op.A, op.B, op.C); break;
    case SUB:     printf("  SUB     $%i, $%i, $%i\n", op.A, op.B, op.C); break;
    case MUL:     printf("  MUL     $%i, $%i, $%i\n", op.A, op.B, op.C); break;
    case DIV:     printf("  DIV     $%i, $%i, $%i\n", op.A, op.B, op.C); break;

    case RET:
      if( op.A )  printf("  RET     $%i\n", op.A); 
      else        printf("  RET\n");
      break;

    default:
      printf("  (%02X)\n", op.op);
      break;
  }
}
