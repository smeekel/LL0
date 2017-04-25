#pragma once
#include "errstate.h"
#include "parser.h"
#include "symboltable.h"
#include "IROp.h"


typedef struct
{
  ErrState      errors;
  SymbolTable   symtab;

} IRGenerator;

int   irgen_initialize  (IRGenerator*);
int   irgen_terminate   (IRGenerator*);
int   irgen_generate    (IRGenerator*, Parser* p);
