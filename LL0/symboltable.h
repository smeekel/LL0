#pragma once


typedef struct
{
  int i;
} SymbolTable;


int   symtab_initialize   (SymbolTable*);
int   symtab_terminate    (SymbolTable*);
