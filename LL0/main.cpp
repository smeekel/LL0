#include <cstdio>
#include "Parser.h"
#include "Lexer.h"
#include "Exception.h"
#include "IStreamFile.h"
#include "Tokens.h"
#include "Utility.h"

using namespace LL0;


void main()
{
  Parser* parser = NULL;

  try
  {
    Lexer l(new IStreamFile("test.txt"));

    Token* t;
    while( (t = l.next())->getType()!=T_EOF )
    {
      printf(": %d\n", t->getType());
      SAFE_DELETE(t);
    }
  }
  catch( const Exception& e )
  {
    // exception
    std::printf("exception [%s]\n", e.getMessage());
  }

  SAFE_DELETE(parser);
}
