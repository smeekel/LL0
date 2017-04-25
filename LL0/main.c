#include <stdio.h>
#include <malloc.h>
#include "parser.h"
#include "irgenerator.h"
#include "string.h"


void main()
{
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
  irgen_print(&irgen);

shutdown:
  irgen_terminate(&irgen);
  parser_terminate(&parser);


  getchar();
}
