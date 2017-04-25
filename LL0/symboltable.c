#include <malloc.h>
#include "symboltable.h"
#include "errstate.h"


static Symbol*  newSymbol     ();
static void     deleteSymbol  (Symbol*);

int symtab_initialize(SymbolTable* p)
{
  llist_initialize(&p->symbols, (llistNodeDelete)deleteSymbol);
  p->lastVIndex   = 0;
  p->currentScope = 0;
  
  return SUCCESS;
}

int symtab_terminate(SymbolTable* p)
{
  llist_terminate(&p->symbols);
  return SUCCESS;
}

Symbol* symtab_new(SymbolTable* p, const char* name)
{
  for( LListNode* i=p->symbols.first ; i ; i=i->next )
  {
    Symbol* symbol = (Symbol*)i;
    if( string_compare_cstr(&symbol->name, name)==0 )
    {
      if( symbol->scope==p->currentScope )
        return NULL;
    }
  }

  Symbol* symbol = newSymbol();
  
  string_copy_cstr(&symbol->name, name);
  symbol->vindex  = ++p->lastVIndex;
  symbol->scope   = p->currentScope;

  llist_add_end(&p->symbols, (LListNode*)symbol);

  return symbol;
}

Symbol* newSymbol()
{
  Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
  string_initialize(&symbol->name);
  llist_node_initialize(&symbol->listData);
  symbol->scope   = 0;
  symbol->vindex  = 0;
  return symbol;
}

void deleteSymbol(Symbol* p)
{
  Symbol* symbol = (Symbol*)p;
  string_terminate(&symbol->name);
  free(symbol);
}

void symtab_scope_push(SymbolTable* p)
{
  p->currentScope++;
}


void symtab_scope_pop(SymbolTable* p)
{
  p->currentScope--;

  for( LListNode* i=p->symbols.first ; i ; i=i->next )
  {
    Symbol* symbol = (Symbol*)i;
    if( symbol->scope>p->currentScope )
    {
      llist_remove(&p->symbols, (LListNode*)symbol);
      deleteSymbol(symbol);
    }
  }
}

int symtab_new_vindex(SymbolTable* p)
{
  return ++p->lastVIndex;
}

Symbol* symtab_find(SymbolTable* p, const char* name)
{
  Symbol* bestCandidate = NULL;

  for( LListNode* i=p->symbols.first ; i ; i=i->next )
  {
    Symbol* symbol = (Symbol*)i;
    if( string_compare_cstr(&symbol->name, name)==0 )
    {
      if( !bestCandidate || symbol->scope>bestCandidate->scope )
      {
        bestCandidate = symbol;
      }
    }
  }

  return bestCandidate;
}
