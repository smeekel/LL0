#include "avltree.h"
#include "errstate.h"


int avltree_initialize(AVLTree* tree, avlnode_compare pfcompare, avlnode_delete pfdelete)
{
  tree->root      = NULL;
  tree->count     = 0;
  tree->ncompare  = pfcompare;
  tree->ndelete   = pfdelete;

  return SUCCESS;
}

int avltree_terminate(AVLTree* tree)
{
  return SUCCESS;
}

int avltree_insert(AVLTree* tree, AVLNode* node)
{
  return SUCCESS;
}
