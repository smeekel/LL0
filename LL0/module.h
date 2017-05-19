#pragma once
#include "llist.h"
#include "function.h"
#include "symboltable.h"
#include "errstate.h"
#include "Imports.h"
#include "Constants.h"


typedef struct
{
  LList       functions;
  SymbolTable symtab;
  Imports     imports;
  Constants   constants;
} Module;

int       module_initialize       (Module*);
int       module_terminate        (Module*);
Function* module_create_function  (Module*, const char* name);
Constant* module_create_constant  (Module*, const char* name, const char* value);
