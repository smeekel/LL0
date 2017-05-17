#pragma once
#include "llist.h"
#include "string.h"

typedef struct
{
  LListNode   node;
  String      name;
  String      as;
} Import;

typedef struct
{
  LList set;  
} Imports;


int     imports_initialize  (Imports*);
int     imports_terminate   (Imports*);
Import* imports_add         (Imports*, const char* name);
Import* imports_add_alias   (Imports*, const char* name, const char* alias);
