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

void llist_add_end(LList* p, LListNode* n)
{
  if( !p->last )
  {
    p->first  = n;
    p->last   = n;
    n->next   = NULL;
    n->prev   = NULL;
    p->count  = 1;
  }
  else
  {
    p->last->next = n;
    n->prev       = p->last;
    p->last       = n;
    p->count++;
  }
}

void llist_remove(LList* p, LListNode* n)
{
  if( n->prev ) n->prev->next = n->next;
  if( n->next ) n->next->prev = n->prev;
  
  if( p->first==n ) p->first = p->first->next;
  if( p->last==n  ) p->last  = p->last->prev;
  
  p->count--;
}

void llist_node_initialize(LListNode* n)
{
  n->next = NULL;
  n->prev = NULL;
}

