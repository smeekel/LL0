#include <stdio.h>
#include <malloc.h>
#include "parser.h"
#include "irgenerator.h"
#include "avltree.h"
#include "string.h"


typedef struct
{
  AVLNode node;
  String  key;
  int     value;
} Derp;

static int derp_compare(const AVLNode* A, const AVLNode* B)
{
  const Derp* AA = (Derp*)A;
  const Derp* BB = (Derp*)B;

  printf("[%s]:[%s]=%d\n", AA->key.buffer, BB->key.buffer, string_compare(&AA->key, &BB->key));
  return string_compare(&AA->key, &BB->key);
}

static void derp_delete(AVLNode* n)
{
  Derp* nn = (Derp*)n;
  string_terminate(&nn->key);
}

static Derp* derp_new(const char* name, const int value)
{
  Derp* n = (Derp*)malloc(sizeof(Derp));
  string_initialize(&n->key);
  string_copy_cstr(&n->key, name);
  return n;
}

static void derp_print(const AVLNode* n, const int depth)
{
  const Derp* nn = (const Derp*)n;
  printf("%*s[%s]\n", depth*2, "", nn->key.buffer);
  if( n->left  ) derp_print(n->left,  depth+1);
  if( n->right ) derp_print(n->right, depth+1);
}

void main()
{
  AVLTree tree;

  avltree_initialize(&tree, derp_compare, derp_delete);
  avltree_insert(&tree, (AVLNode*)derp_new("10", 123));
  avltree_insert(&tree, (AVLNode*)derp_new("20", 123));
  avltree_insert(&tree, (AVLNode*)derp_new("30", 123));
  avltree_insert(&tree, (AVLNode*)derp_new("40", 123));
  avltree_insert(&tree, (AVLNode*)derp_new("50", 123));
  avltree_insert(&tree, (AVLNode*)derp_new("25", 123));
  derp_print(tree.root, 0);

  avltree_terminate(&tree);

  /*
  Parser parser;
  IRGenerator irgen;


  parser_initialize(&parser, "testcases/test2.txt");
  irgen_initialize(&irgen);


  if( !parser_generate(&parser) )
  {
    printf("PARSE ERROR: %s\n", errstate_getmessage((PError)&parser));
    goto shutdown;
  }
  if( !irgen_generate(&irgen, &parser) )
  {
    printf("IRGEN ERROR: %s\n", errstate_getmessage((PError)&irgen));
    goto shutdown;
  }

shutdown:
  irgen_terminate(&irgen);
  parser_terminate(&parser);
  */


  getchar();
}
