#pragma once
#include <stdbool.h>
#include <stdint.h>


typedef struct RBTree RBTree;
typedef struct RBNode RBNode;
typedef int   (*rbnode_compare) (const RBNode*, const RBNode*);
typedef void  (*rbnode_delete)  (RBNode*);

struct RBNode
{
  RBNode* parent;
  RBNode* left;
  RBNode* right;
  bool   color;
};

struct RBTree
{
  RBNode*         root;
  uint32_t        count;
  rbnode_compare  ncompare;
  rbnode_delete   ndelete;
};

int   rbtree_initialize   (RBTree*, rbnode_compare, rbnode_delete);
int   rbtree_terminate    (RBTree*);
int   rbtree_add          (RBTree*, RBNode*);
