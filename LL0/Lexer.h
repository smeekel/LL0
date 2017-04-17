#pragma once
#include "errstate.h"
#include "InputStream.h"


typedef struct
{
  ErrState      errorState;
  InputStream*  input;
} LexerState;


int   lexer_initialize  (LexerState*, const char* filename);
int   lexer_terminate   (LexerState*);
int   lexer_peek        (LexerState*);
int   lexer_next        (LexerState*);

