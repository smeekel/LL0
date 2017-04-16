#pragma once
#include "types.h"
#include "errstate.h"


typedef struct
{
  ErrState errorState;

} LexerState;

typedef LexerState* const PLexer;


int   lexer_Initialize  (PLexer);
int   lexer_Terminate   (PLexer);
int   lexer_Peek        (PLexer);
int   lexer_Next        (PLexer);

