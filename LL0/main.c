#include <stdio.h>
#include <malloc.h>
#include "parser.h"
#include "irgenerator.h"
#include "string.h"
#include "vm.h"


void main()
{
  Parser      parser;
  IRGenerator irgen;
  VMState     vm;


  parser_initialize(&parser, "testcases/test3.txt");
  irgen_initialize(&irgen);
  vm_initialize(&vm);


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
  vm_run(&vm, &irgen.module);


shutdown:
  vm_terminate(&vm);
  irgen_terminate(&irgen);
  parser_terminate(&parser);


  getchar();
}
