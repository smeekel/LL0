#include <malloc.h>
#include "Constants.h"
#include "errstate.h"


static void list_constant_delete(Constant*);


int constants_initialize(Constants* p)
{
  llist_initialize(&p->set, (llistNodeDelete)list_constant_delete);
  p->index = 0;
  return SUCCESS;
}

int constants_terminate(Constants* p)
{
  llist_terminate(&p->set);
  return SUCCESS;
}


Constant* constants_pin(Constants* p, const String* str)
{
  for( LListNode* n=p->set.first ; n ; n=n->next )
  {
    Constant* k = (Constant*)n;

    if( !string_compare(&k->value, str) )
      return k;
  }

  Constant* k = (Constant*)malloc(sizeof(Constant));
  constant_initialize(k);
  llist_add_end(&p->set, (LListNode*)k);
  string_copy(str, &k->value);
  k->index = ++p->index;

  return k;
}



void list_constant_delete(Constant* p)
{
  constant_terminate(p);
  free(p);
}
