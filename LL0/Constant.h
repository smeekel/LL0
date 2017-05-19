#pragma once
#include "llist.h"
#include "string.h"

typedef struct
{
  LListNode node;
  int       index;
  String    value;
  String    name;
} Constant;

int   constant_initialize (Constant*);
int   constant_terminate  (Constant*);

