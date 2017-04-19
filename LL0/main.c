#include <stdio.h>
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


void main()
{
  RBTree tree;

  rbtree_initialize(&tree);


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
