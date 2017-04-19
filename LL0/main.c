#include <stdio.h>
#include "parser.h"
#include "InputStream.h"



void main()
{
  ParserState parser;

  parser_initialize (&parser, "testcases/test2.txt");
  parser_generate   (&parser);
  parser_terminate  (&parser);

  getchar();
}
