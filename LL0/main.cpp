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
  Parser parser(new IStreamFile("testcases/test2.txt"));

  try
  {
    parser.test();
  }
  catch( const Exception& e )
  {
    // exception
    std::printf("exception [%s]\n", e.getMessage());
  }

}
