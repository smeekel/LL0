#pragma once
#include "string.h"
#include "llist.h"


typedef struct Symbol Symbol;

struct Symbol
{
  LListNode listData;
  String    name;
  int       scope;
  int       vindex;
};

typedef struct
{
  LList   symbols;
  int     currentScope;
  int     lastVIndex;
} SymbolTable;


int       symtab_initialize   (SymbolTable*);
int       symtab_terminate    (SymbolTable*);
Symbol*   symtab_new          (SymbolTable*, const char* name);
void      symtab_scope_push   (SymbolTable*);
void      symtab_scope_pop    (SymbolTable*);
int       symtab_new_vindex   (SymbolTable*);
Symbol*   symtab_find         (SymbolTable*, const char* name);
