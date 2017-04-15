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
  gTop(parseTree.get());

  for( auto i=symbolTable.begin() ; i!=symbolTable.end() ; i++ )
  {
    
  }
}

void IRGenerator::gTop(const Node* n)
{
  if( n->is(N_GLUE) )
  {
    gTop(n->getA());
    gTop(n->getB());
  }
  else if( n->is(N_VAR) )
  {
    gVar(n);
  }
  else
  {
    int temp = gEval(n);
    printf("DIS $%i\n", temp); //temp
  }
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
    printf("MOV $%i, $%i\n", temp, sym.getTemp());
  }
}

int IRGenerator::gEval(const Node* n)
{
  if( n->is(N_N_LITERAL) )
  {
    const int temp = ++varIndex;
    printf("LOADK $%i, %s\n", temp, n->getRaw());
    return temp;
  }
  else if( n->is(N_IDENT) )
  {
    const Symbol s = findSymbol(n->getRaw());
    
    if( s.isNull() )
      throw IR_EXCEPTION("Unknown variable [%s]", n->getRaw());

    return s.getTemp();
  }
  else if( n->is(N_PLUS) )
  {
    const int temp = ++varIndex;
    const int a = gEval(n->getA());
    const int b = gEval(n->getB());

    printf("ADD $%i, $%i, $%i\n", a, b, temp);

    return temp;
  }
  else if( n->is(N_MINUS) )
  {
    const int temp = ++varIndex;
    const int a = gEval(n->getA());
    const int b = gEval(n->getB());

    printf("SUB $%i, $%i, $%i\n", a, b, temp);

    return temp;
  }
  else if( n->is(N_MUL) )
  {
    const int temp = ++varIndex;
    const int a = gEval(n->getA());
    const int b = gEval(n->getB());

    printf("MUL $%i, $%i, $%i\n", a, b, temp);

    return temp;
  }
  else if( n->is(N_DIV) )
  {
    const int temp = ++varIndex;
    const int a = gEval(n->getA());
    const int b = gEval(n->getB());

    printf("DIV $%i, $%i, $%i\n", a, b, temp);

    return temp;
  }
  else if( n->is(N_CALL) )
  {
    const Node* name = n->getA();
    if( !name->is(N_IDENT) )
      throw EXCEPTION("Compiler error: N_CALL/A != N_IDENT");

    const int count = countParameters(n->getB());
    
    gPushCallParameters(n->getB());
    printf("CALL \"%s\", %d\n", name->getRaw(), count);

    if( count>0 )
      printf("POP %d\n", count);

  }
  else
  {
    printf("> UN=%s\n", nodeTypeToString(n->getType()));
  }
  
  return 0;
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
    printf("PUSH $%i\n", temp);
  }
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
