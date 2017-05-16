#pragma once
#include "errstate.h"
#include "parser.h"
#include "symboltable.h"
#include "llist.h"
#include "IROp.h"


typedef struct 
{
  LListNode node;
  String    name;
  LList     ops;
} IRFunction;

typedef struct
{
  ErrState      errors;
  SymbolTable   symtab;
  int           labelIndex;

  LList         functions;
  IRFunction*   current;
  IRFunction*   global;

} IRGenerator;

int   irgen_initialize  (IRGenerator*);
int   irgen_terminate   (IRGenerator*);
int   irgen_generate    (IRGenerator*, Parser* p);
void  irgen_print       (IRGenerator*);
