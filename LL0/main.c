#include "lexer.h"
#include "InputStream.h"



void main()
{
  LexerState lex;

  lexer_initialize(&lex, "testcases/test2.txt");
  lexer_terminate(&lex);
}
