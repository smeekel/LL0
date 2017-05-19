#pragma once
#include "llist.h"
#include "Constant.h"


typedef struct
{
  LList set;
  int   index;
} Constants;

int   constants_initialize  (Constants*);
int   constants_terminate   (Constants*);
Constant* constants_pin     (Constants*, const String*);
