#include <cstdio>
#include "Exception.h"
#include "IStreamFile.h"
#include "Parser.h"
#include "IRGenerator.h"

using namespace LL0;


void main()
{

  try
  {
    Parser      parser(new IStreamFile("testcases/test2.txt"));
    SafeNode    parseTree;
    IRGenerator irgen;

    parseTree = parser.generate();
    irgen.generate(parseTree);

  }
  catch( const Exception& e )
  {
    // exception
    std::printf("exception [%s]\n", e.getMessage());
  }

  std::getchar();
}
