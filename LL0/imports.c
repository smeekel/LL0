#include <malloc.h>
#include "errstate.h"
#include "Imports.h"


static Import*  import_alloc();
static void     import_delete(Import*);


int imports_initialize(Imports* p)
{
  llist_initialize(&p->set, (llistNodeDelete)import_delete);
  return SUCCESS;
}

int imports_terminate(Imports* p)
{
  return SUCCESS;
}

Import* imports_add(Imports* p, const char* name)
{
  Import* o = import_alloc();

  string_copy_cstr(&o->name, name);
  llist_add_end(&p->set, (LListNode*)o);

  return o;
}

Import* imports_add_alias(Imports* p, const char* name, const char* alias)
{
  Import* o = imports_add(p, name);

  string_copy_cstr(&o->as, alias);

  return o;
}

Import* import_alloc()
{
  Import* o = (Import*)malloc(sizeof(Import));

  llist_node_initialize(&o->node);
  string_initialize(&o->name);
  string_initialize(&o->as);

  return o;
}

void import_delete(Import* p)
{
  string_terminate(&p->name);
  string_terminate(&p->as);
  free(p);
}


