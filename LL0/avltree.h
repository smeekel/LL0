#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "errstate.h"


typedef struct AVLTree AVLTree;
typedef struct AVLNode AVLNode;
typedef int   (*avlnode_compare) (const AVLNode*, const AVLNode*);
typedef void  (*avlnode_delete)  (AVLNode*);

struct AVLNode
{
  AVLNode*  parent;
  AVLNode*  left;
  AVLNode*  right;
  int       height;
};

struct AVLTree
{
  ErrState        errors;
  AVLNode*        root;
  avlnode_compare ncompare;
  avlnode_delete  ndelete;
};

int   avltree_initialize  (AVLTree*, avlnode_compare, avlnode_delete);
int   avltree_terminate   (AVLTree*);
int   avltree_insert      (AVLTree*, AVLNode*);
int   avltree_delete      (AVLTree*, AVLNode*);
void  avltree_clear       (AVLTree*);
