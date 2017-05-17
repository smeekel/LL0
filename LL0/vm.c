#include "vm.h"


int vm_initialize(VMState* p)
{
  errstate_initialize(&p->errors);
  return SUCCESS;
}

int vm_terminate(VMState* p)
{
  errstate_terminate(&p->errors);
  return SUCCESS;
}

int vm_run(VMState* p, Module* m)
{
  return SUCCESS;
}

