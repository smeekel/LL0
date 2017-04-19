#pragma once
#include <stdbool.h>
#include "string.h"


typedef struct
{
  String  name;
  int     scope;
  int     vid;
} Symbol;

Symbol* sym_alloc   ();
void    sym_delete  (Symbol*);
bool    sym_isnull  (Symbol*);
