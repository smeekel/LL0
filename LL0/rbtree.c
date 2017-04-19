#include "rbtree.h"
#include "errstate.h"


int rbtree_initialize(RBTree* tree, rbnode_compare pfcompare, rbnode_delete pfdelete)
{
  tree->root      = NULL;
  tree->count     = 0;
  tree->ncompare  = pfcompare;
  tree->ndelete   = pfdelete;

  return SUCCESS;
}

int rbtree_terminate(RBTree* tree)
{
  return SUCCESS;
}

int rbtree_add(RBTree* tree, RBNode* node)
{
  return SUCCESS;
}
