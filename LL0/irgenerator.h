#pragma once
#include "errstate.h"
#include "parser.h"
#include "irmodule.h"


typedef struct
{
  ErrState  errors;
  IRModule  module;
} IRGenerator;

int   irgen_initialize  (IRGenerator*);
int   irgen_terminate   (IRGenerator*);
int   irgen_generate    (IRGenerator*, Parser* p);
