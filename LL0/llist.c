#include "llist.h"

void llist_initialize(LList* p, llistNodeDelete deleteCallback)
{
  p->first    = NULL;
  p->last     = NULL;
  p->count    = 0;
  
  p->cbDelete = deleteCallback;
}

void llist_terminate(LList* p)
{
  for( LListNode* i=p->first ; i ; )
  {
    LListNode* next = i->next;
    p->cbDelete(i);
    i = next;
  }
}

