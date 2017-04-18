#pragma once
#include "errstate.h"
#include "InputStream.h"
#include "string.h"


typedef struct
{
  ErrState      errorState;
  InputStream*  input;
  
  int           c;
  uint32_t      line;
  uint32_t      column;
  String        tokenraw;
} LexerState;


int   lexer_initialize  (LexerState*, const char* filename);
int   lexer_terminate   (LexerState*);
int   lexer_peek        (LexerState*);
int   lexer_next        (LexerState*);

