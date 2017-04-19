#pragma once
#include <stdbool.h>
#include <stdint.h>


typedef struct RBTree RBTree;
typedef struct RBNode RBNode;
typedef int (*rbnode_compare)(const RBNode* A, const RBNode* b);

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
  rbnode_compare  fcompare;
};

int   rbtree_initialize   (RBTree*);
int   rbtree_terminate    (RBTree*);
