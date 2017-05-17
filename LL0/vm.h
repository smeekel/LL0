#pragma once
#include "module.h"
#include "errstate.h"


typedef struct
{
  ErrState      errors;

} VMState;

int   vm_initialize (VMState*);
int   vm_terminate  (VMState*);
int   vm_run        (VMState*, Module*);
