#pragma once
#include "lexer.h"
#include "errstate.h"
#include "node.h"


typedef struct
{
  ErrState    error;
  Lexer       lexer;
  Node*       root;
} Parser;


int   parser_initialize (Parser*, const char* filename);
int   parser_terminate  (Parser*);
int   parser_generate   (Parser*);
