#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "errstate.h"
#include "InputStream.h"
#include "string.h"


typedef struct
{
  ErrState      error;
  InputStream*  input;
  
  int           c;
  int           token;
  uint32_t      line;
  uint32_t      column;
  String        raw;

  int           current_token;
  String        current_raw;
  uint32_t      current_line;
  uint32_t      current_column;
} LexerState;


int   lexer_initialize  (LexerState*, const char* filename);
int   lexer_terminate   (LexerState*);
int   lexer_peek        (LexerState*);
int   lexer_next        (LexerState*);

