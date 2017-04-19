#pragma once
#include <stdbool.h>
#include <stdint.h>


typedef struct AVLTree AVLTree;
typedef struct AVLNode AVLNode;
typedef int   (*avlnode_compare) (const AVLNode*, const AVLNode*);
typedef void  (*avlnode_delete)  (AVLNode*);

struct AVLNode
{
  AVLNode* parent;
  AVLNode* left;
  AVLNode* right;
};

struct AVLTree
{
  AVLNode*        root;
  uint32_t        count;
  avlnode_compare ncompare;
  avlnode_delete  ndelete;
};

int   avltree_initialize  (AVLTree*, avlnode_compare, avlnode_delete);
int   avltree_terminate   (AVLTree*);
int   avltree_insert      (AVLTree*, AVLNode*);
