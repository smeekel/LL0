#include "IRGenerator.h"
#include "Exception.h"

using namespace LL0;

#define IR_EXCEPTION(message, ...) EXCEPTION("(%d:%d) " message, n->getLine(), n->getColumn(), __VA_ARGS__)


IRGenerator::IRGenerator()
{
}

IRGenerator::~IRGenerator()
{
}

void IRGenerator::generate(SafeNode parseTree)
{
  gBlock(parseTree.get());
  ir.print();
}

void IRGenerator::gBlock(const Node* n)
{
  switch( n->getType() )
  {
    case N_GLUE:
      gBlock(n->getA());
      gBlock(n->getB());
      break;

    case N_VAR:       gVar(n);        break;
    case N_CALL:      gCall(n);       break;
    case N_IF:        gIf(n);         break;
    case N_FUNCTION:  gFunction(n);   break;
    case N_RETURN:    gReturn(n);     break;

    default:
    {
      gEval(n);
    }
  }

}

void IRGenerator::gFunction(const Node* n)
{
  const Node* ident   = n->getA();
  const Node* params  = n->getB();
  const Node* body    = n->getC();

  ir.sym.scopePush();
  printf("@%s:\n", ident->getRaw());
  
  int temp = 0;
  gFunctionPushParams(params, temp);

  gBlock(body);
  ir.addOp(RET);

  ir.sym.scopePop();
}

void IRGenerator::gFunctionPushParams(const Node* n, int& index)
{
  if( n->is(N_GLUE) )
  {
    gFunctionPushParams(n->getA(), index);
    gFunctionPushParams(n->getB(), index);
  }
  else
  {
    Symbol param = ir.sym.newSymbol(n->getRaw());
    if( param.isNull() )
      throw IR_EXCEPTION("Duplicate parameter name [%s]", n->getRaw());

    ir.addOp(SMOV, index++, param.vindex);
  }
}

void IRGenerator::gReturn(const Node* n)
{
  const Node* expr = n->getA();

  if( expr )
  {
    const int temp = gEval(expr);
    ir.addOp(PUSH, temp);
    ir.addOp(RET, 1);
  }
  else
  {
    ir.addOp(RET);
  }

}

void IRGenerator::gVar(const Node* n)
{
  const Node*   ident       = n->getA();
  const Node*   expression  = n->getB();
  const Symbol  sym         = ir.sym.newSymbol(ident->getRaw());

  if( sym.isNull() )
    throw IR_EXCEPTION("Duplicate variable name [%s]", ident->getRaw());

  if( expression )
  {
    int temp = gEval(expression);
    ir.addOp(MOV, temp, sym.vindex);
  }
}

int IRGenerator::gEval(const Node* n)
{
  switch( n->getType() )
  {
    case N_N_LITERAL:   return gLiteral(n);
    case N_IDENT:       return gIdent(n);
    case N_CALL:        return gCall(n);

    case N_ASSIGNMENT:  return gAssignment(n);
    case N_PLUS:        return gPlus(n);
    case N_MINUS:       return gMinus(n);
    case N_MUL:         return gMul(n);
    case N_DIV:         return gDiv(n);

    default:
      printf("> UN=%s\n", nodeTypeToString(n->getType()));
      return 0;
  }

}

int IRGenerator::gAssignment(const Node* n)
{
  const Node* lparam  = n->getA();
  const Node* rparam  = n->getB();

  if( !lparam->is(N_IDENT) )
    throw IR_EXCEPTION("Invalid lparam. Must be identifier");
  
  const Symbol sym = ir.sym.findSymbol(lparam->getRaw());
  if( sym.isNull() )
    throw IR_EXCEPTION("Undefined variable [%s]", lparam->getRaw());

  const int expr_temp = gEval(rparam);
  ir.addOp(MOV, expr_temp, sym.vindex);

  return sym.vindex;
}

int IRGenerator::gCall(const Node* n)
{
  const Node* name = n->getA();
  const int   temp = ir.newVirtualIndex();

  if( !name->is(N_IDENT) )
    throw IR_EXCEPTION("Compiler error: N_CALL/A != N_IDENT");
  
  Symbol sname = ir.sym.newSymbol(name->getRaw());
  if( sname.isNull() )
    throw IR_EXCEPTION("Duplicate identifier [%s]", name->getRaw());

  const int count = countParameters(n->getB());
    
  gPushCallParameters(n->getB());

  ir.addOp(CALL, sname.vindex, count);
  ir.addOp(SMOV, 0, temp);
  ir.addOp(POP, count+1);

  return temp; //debug
}

void IRGenerator::gPushCallParameters(const Node* n)
{
  if( n->is(N_NEXT) )
  {
    gPushCallParameters(n->getA());
    gPushCallParameters(n->getB());
  }
  else
  {
    const int temp = gEval(n);
    ir.addOp(PUSH, temp);
  }
}

int IRGenerator::gIf(const Node* n)
{
  const Node* condition = n->getA();
  const Node* onTrue    = n->getB();
  const Node* onFalse   = n->getC();

  const int expTemp     = gEval(condition);
  const int labelFalse  = ir.newVirtualIndex();
  ir.addOp(JMPF, labelFalse, expTemp);

  gBlock(onTrue);

  if( !onFalse )
  {
    printf("@L%i:\n", expTemp);
  }
  else
  {
    const int labelExit = ir.newVirtualIndex();
    ir.addOp(JMP, labelExit);
    printf("@L%i:\n", expTemp);

    gBlock(onFalse);

    printf("@L%i:\n", labelExit);
  }
  
  return 0;
}

int IRGenerator::gLiteral(const Node* n)
{
  const int temp = ir.newVirtualIndex();
  
  IROp op(LOADK);
  op.raw  = n->getRaw();
  op.B    = temp;

  ir.addOp(op);
  
  return temp;
}

int IRGenerator::gIdent(const Node* n)
{
  const Symbol s = ir.sym.findSymbol(n->getRaw());
    
  if( s.isNull() )
    throw IR_EXCEPTION("Unknown variable [%s]", n->getRaw());

  return s.vindex;
}

int IRGenerator::gPlus(const Node* n)
{
  const int temp = ir.newVirtualIndex();
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  ir.addOp(ADD, a, b, temp);

  return temp;
}

int IRGenerator::gMinus(const Node* n)
{
  const int temp = ir.newVirtualIndex();
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  ir.addOp(SUB, a, b, temp);

  return temp;
}

int IRGenerator::gMul(const Node* n)
{
  const int temp = ir.newVirtualIndex();
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  ir.addOp(MUL, a, b, temp);

  return temp;
}

int IRGenerator::gDiv(const Node* n)
{
  const int temp = ir.newVirtualIndex();
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  ir.addOp(DIV, a, b, temp);

  return temp;
}

int IRGenerator::countParameters(const Node* n)
{
  if( n->is(N_NEXT) )
  {
    return 
      countParameters(n->getA())
      +
      countParameters(n->getB())
      ;
  }
  else
  {
    return 1;
  }
}

