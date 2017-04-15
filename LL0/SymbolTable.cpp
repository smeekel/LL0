#include "SymbolTable.h"

using namespace LL0;


SymbolTable::SymbolTable()
{
  scope         = 0;
  virutalIndex  = 0;
}

int SymbolTable::newVirtualIndex()
{
  return ++virutalIndex;
}

Symbol SymbolTable::newSymbol(const char* name)
{
  Symbol s = findLocalSymbol(name);

  if( !s.isNull() )
    return Symbol::null();

  s.name    = name;
  s.vindex  = newVirtualIndex();
  s.scope   = scope;

  table.insert(SymbolPair(s.name, s));

  return s;
}

Symbol SymbolTable::findSymbol(const char* name)
{
  Symbol s;

  for( auto i=table.find(name) ; i!=table.end() ; i++ )
  {
    if( i->second.scope>s.scope )
      s = i->second;
  }

  return s;
}

Symbol SymbolTable::findLocalSymbol(const char* name)
{
  for( auto i=table.find(name) ; i!=table.end() ; i++ )
  {
    if( i->second.scope==this->scope )
      return i->second;
  }

  return Symbol::null();
}

void SymbolTable::scopePush()
{
  scope++;
}

void SymbolTable::scopePop()
{
  if( scope<=0 ) return;

  for( auto i=table.begin() ; i!=table.end() ; )
  {
    if( i->second.scope>=this->scope )
      table.erase(i++);
    else
      i++;
  }
  scope--;
}

