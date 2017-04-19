#pragma once

typedef struct
{
  int i;
} IRModule;

int   irmod_initialize  (IRModule*);
int   irmod_terminate   (IRModule*);

