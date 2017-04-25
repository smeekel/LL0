#pragma once
#include <stdint.h>


typedef struct LListNode LListNode;

struct LListNode
{
  LListNode*  prev;
  LListNode*  next;
};

typedef void (*llistNodeDelete)(LListNode*);

typedef struct 
{
  LListNode*      first;
  LListNode*      last;
  uint32_t        count;
  llistNodeDelete cbDelete;
} LList;

void  llist_initialize      (LList*, llistNodeDelete);
void  llist_terminate       (LList*);
void  llist_node_initialize (LListNode*);
void  llist_add_end         (LList*, LListNode*);
void  llist_remove          (LList*, LListNode*);
