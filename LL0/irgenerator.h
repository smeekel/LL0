#pragma once
#include "errstate.h"
#include "parser.h"
#include "module.h"
#include "llist.h"
#include "IROp.h"


typedef struct
{
  ErrState      errors;
  int           labelIndex;

  Module        module;

  Function*     current;
  Function*     global;

} IRGenerator;

int   irgen_initialize  (IRGenerator*);
int   irgen_terminate   (IRGenerator*);
int   irgen_generate    (IRGenerator*, Parser* p);
void  irgen_print       (IRGenerator*);
