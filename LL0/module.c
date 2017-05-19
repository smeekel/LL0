#include <malloc.h>
#include "module.h"
#include "IROp.h"


static void         listDeleteFunction  (Function*);
static void         listDeleteOp        (IROp*);


int module_initialize(Module* p)
{
  llist_initialize(&p->functions, (llistNodeDelete)listDeleteFunction);
  symtab_initialize(&p->symtab);
  imports_initialize(&p->imports);
  constants_initialize(&p->constants);
  return SUCCESS;
}

int module_terminate(Module* p)
{
  constants_terminate(&p->constants);
  imports_terminate(&p->imports);
  llist_terminate(&p->functions);
  symtab_terminate(&p->symtab);
  return SUCCESS;
}

void listDeleteOp(IROp* p)
{
  string_terminate(&p->raw);
  free(p);
}

void listDeleteFunction(Function* p)
{
  llist_terminate (&p->ops);
  string_terminate(&p->name);
  free(p);
}

Function* module_create_function(Module* p, const char* name)
{
  Function* fn = (Function*)malloc(sizeof(Function));

  llist_node_initialize((LListNode*)fn);
  llist_initialize(&fn->ops, (llistNodeDelete)listDeleteOp);
  string_initialize(&fn->name);
  llist_add_end(&p->functions, (LListNode*)fn);
  
  if( name!=NULL ) string_copy_cstr(&fn->name, name);

  return fn;
}


