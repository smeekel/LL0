#pragma once
#include "string.h"
#include "llist.h"


typedef struct
{
  LListNode node;
  String    name;
  LList     ops;
} Function;

