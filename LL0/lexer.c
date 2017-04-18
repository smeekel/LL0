#include "lexer.h"


#define READCHAR()  (p->input->readchar(p->input))
#define PEEKCHAR()  (p->input->peekchar(p->input))

static int lexer_nextchar(LexerState*);


int lexer_initialize(LexerState* p, const char* filename)
{
  errstate_initialize(&p->errorState);
  string_initialize(&p->tokenraw);

  p->c      = 0;
  p->line   = 1;
  p->column = 0;

  p->input = is_openfile(filename);

  if( !p->input )
  {
    errstate_adderror(&p->errorState, "Failed to open lexer input file");
    return ERROR;
  }

  lexer_nextchar(p);  // prime input stream
  lexer_next(p);      // prime look-ahead

  return SUCCESS;
}

int lexer_terminate(LexerState* p)
{
  string_terminate  (&p->tokenraw);
  errstate_terminate(&p->errorState);
  return SUCCESS;
}

int lexer_nextchar(LexerState* p)
{
  p->c = READCHAR();
  p->column++;
  return (p->input==0);
}

int lexer_next(LexerState* p)
{
  string_clear(&p->tokenraw);

  if( p->input->iseof(p->input) )
    return 0;

restart:
  switch( p->c )
  {
    case '\t':    case '\v':
    case '\r':    case ' ':
    case '\x0C':  case '\x85':
      lexer_nextchar(p);
      goto restart;

    case '\n':
      lexer_newline(p);
      lexer_nextchar(p);
      goto restart;

    case '/':
    {
      const char peek = PEEKCHAR();
      if( peek=='/' || peek=='*' )
      {
      }
    }
  }

  return 0;
}

