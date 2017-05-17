#pragma once
#include "llist.h"
#include "function.h"
#include "symboltable.h"
#include "errstate.h"
#include "Imports.h"


typedef struct
{
  LList       functions;
  SymbolTable symtab;
  Imports     imports;
} Module;

int       module_initialize       (Module*);
int       module_terminate        (Module*);
Function* module_create_function  (Module*, const char* name);
