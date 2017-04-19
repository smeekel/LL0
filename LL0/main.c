#include <stdio.h>
#include <malloc.h>
#include "parser.h"
#include "irgenerator.h"
#include "rbtree.h"
#include "string.h"


typedef struct
{
  RBNode  node;
  String  key;
  int     value;
} Derp;

static int derp_compare(const RBNode* A, const RBNode* B)
{
  return 0;
}

static void derp_delete(RBNode* n)
{
  Derp* nn = (Derp*)n;
  string_terminate(&nn->key);
}

static Derp* derp_new(const char* name, const int value)
{
  Derp* n = (Derp*)malloc(sizeof(Derp));
  string_initialize(&n->key);
  string_copy_cstr(&n->key, name);
}


void main()
{
  RBTree tree;

  rbtree_initialize(&tree, derp_compare, derp_delete);


  rbtree_terminate(&tree);

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
