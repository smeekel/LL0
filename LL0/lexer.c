#include "lexer.h"


int lexer_Initialize(PLexer p)
{
  errstate_Initialize((PError)p);
  return 0;
}

int lexer_Terminate(PLexer p)
{
  errstate_Terminate((PError)p);
  return 0;
}


