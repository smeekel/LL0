#include "Constant.h"
#include "errstate.h"


int constant_initialize(Constant* p)
{
  llist_node_initialize(&p->node);
  string_initialize(&p->name);
  string_initialize(&p->value);
  p->index = 0;
  return SUCCESS;
}

int constant_terminate(Constant* p)
{
  string_terminate(&p->name);
  string_terminate(&p->value);
  return SUCCESS;
}
