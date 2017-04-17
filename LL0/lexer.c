#include "lexer.h"


int lexer_initialize(LexerState* p, const char* filename)
{
  errstate_initialize(&p->errorState);
  p->input = is_openfile(filename);

  if( !p->input )
  {
    errstate_adderror(&p->errorState, "Failed to open lexer input file");
    return ERROR;
  }

  

  return SUCCESS;
}

int lexer_terminate(LexerState* p)
{
  errstate_terminate(&p->errorState);
  return SUCCESS;
}


