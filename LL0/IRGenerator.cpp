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
  //parseTree->print();

  symbolTable.clear();

  varIndex = 0;

  printf(".START\n");

  gBlock(parseTree.get());

  printf("\n");
  for( auto i=symbolTable.begin() ; i!=symbolTable.end() ; i++ )
  {
    printf(".VAR [%s] -> $%i\n", i->second.name.c_str(), i->second.temp);
  }
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

  printf("@%s:\n", ident->getRaw());
  gBlock(body);
  printf("  RET\n");

}

void IRGenerator::gReturn(const Node* n)
{
  const Node* expr = n->getA();

  if( expr )
  {
    const int temp = gEval(expr);
    printf("  PUSH $%i\n", temp);
    printf("  RET 1\n");
  }
  else
  {
    printf("  RET\n");
  }

  //const int temp = gEval;

}

void IRGenerator::gVar(const Node* n)
{
  const Node*   ident       = n->getA();
  const Node*   expression  = n->getB();
  const Symbol  sym         = newSymbol(ident->getRaw());

  if( sym.isNull() )
    throw IR_EXCEPTION("Duplicate variable name [%s]", ident->getRaw());

  if( expression )
  {
    int temp = gEval(expression);
    printf("  MOV $%i, $%i\n", temp, sym.getTemp());
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
  
  const Symbol sym = findSymbol(lparam->getRaw());
  if( sym.isNull() )
    throw IR_EXCEPTION("Undefined variable [%s]", lparam->getRaw());

  const int expr_temp = gEval(rparam);
  printf("  MOV $%i, $%i\n", expr_temp, sym.temp);

  return sym.temp;
}

int IRGenerator::gCall(const Node* n)
{
  const Node* name = n->getA();
  const int   temp = ++varIndex;

  if( !name->is(N_IDENT) )
    throw EXCEPTION("Compiler error: N_CALL/A != N_IDENT");

  const int count = countParameters(n->getB());
    
  gPushCallParameters(n->getB());
  printf("  CALL \"%s\", %d\n", name->getRaw(), count);
  printf("  POP $%i\n", temp);
  printf("  SWEEP %i\n", count);

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
    printf("  PUSH $%i\n", temp);
  }
}

int IRGenerator::gIf(const Node* n)
{
  const Node* condition = n->getA();
  const Node* onTrue    = n->getB();
  const Node* onFalse   = n->getC();

  const int expTemp     = gEval(condition);
  const int labelFalse  = ++varIndex;
  printf("  JMPF @L%i <$%i>\n", labelFalse, expTemp);

  gBlock(onTrue);

  if( !onFalse )
  {
    printf("@L%i:\n", expTemp);
  }
  else
  {
    const int labelExit = ++varIndex;
    printf("  JMP @L%i\n", labelExit);
    printf("@L%i:\n", expTemp);

    gBlock(onFalse);

    printf("@L%i:\n", labelExit);
  }
  
  return 0;
}

int IRGenerator::gLiteral(const Node* n)
{
  const int temp = ++varIndex;
  printf("  LOADK n %s, $%i\n", n->getRaw(), temp);
  return temp;
}

int IRGenerator::gIdent(const Node* n)
{
  const Symbol s = findSymbol(n->getRaw());
    
  if( s.isNull() )
    throw IR_EXCEPTION("Unknown variable [%s]", n->getRaw());

  return s.getTemp();
}

int IRGenerator::gPlus(const Node* n)
{
  const int temp = ++varIndex;
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  printf("  ADD $%i, $%i, $%i\n", a, b, temp);

  return temp;
}

int IRGenerator::gMinus(const Node* n)
{
  const int temp = ++varIndex;
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  printf("  SUB $%i, $%i, $%i\n", a, b, temp);

  return temp;
}

int IRGenerator::gMul(const Node* n)
{
  const int temp = ++varIndex;
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  printf("  MUL $%i, $%i, $%i\n", a, b, temp);

  return temp;
}

int IRGenerator::gDiv(const Node* n)
{
  const int temp = ++varIndex;
  const int a = gEval(n->getA());
  const int b = gEval(n->getB());

  printf("  DIV $%i, $%i, $%i\n", a, b, temp);

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

IRGenerator::Symbol IRGenerator::newSymbol(const char* name)
{
  Symbol s;

  if( !findSymbol(name).isNull() )
    return Symbol::null();

  s.name  = name;
  s.temp  = ++varIndex;
  symbolTable.insert(SymbolPair(s.name, s));

  return s;
}

IRGenerator::Symbol IRGenerator::findSymbol(const char* name)
{
  std::string key(name);
  auto i = symbolTable.find(key);

  return ( i!=symbolTable.end() ) ? i->second : Symbol::null() ;
}
