#pragma once
#include "lexer.h"
#include "errstate.h"
#include "node.h"


typedef struct
{
  ErrState    error;
  LexerState  lexer;
  Node*       root;
} ParserState;


int   parser_initialize (ParserState*, const char* filename);
int   parser_terminate  (ParserState*);
int   parser_generate   (ParserState*);
