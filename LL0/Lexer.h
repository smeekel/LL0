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
} Lexer;


int   lexer_initialize  (Lexer*, const char* filename);
int   lexer_terminate   (Lexer*);
int   lexer_peek        (Lexer*);
int   lexer_next        (Lexer*);

