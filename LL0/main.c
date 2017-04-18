#include <stdio.h>
#include "lexer.h"
#include "InputStream.h"



void main()
{
  LexerState lex;

  lexer_initialize(&lex, "testcases/test2.txt");
  while( true )
  {
    const int token = lexer_next(&lex);
    printf("L %d\n", token);
    if( !token ) break;
    if( token<0 )
    {
      printf("%s\n", lex.error.message);
      break;
    }
  }
  lexer_terminate(&lex);
  getchar();
}
