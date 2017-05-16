#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "string.h"


enum NodeTypes
{
  N_GLUE = 1,
  N_BLOCK,
  N_EXPRESSION,
  N_CALL,
  N_ANONYMOUS,
  N_NEXT,
  N_DEREF,

  N_LITERAL,
  N_IDENT,

  N_IMPORT,
  N_FUNCTION,
  N_VAR,
  N_RETURN,
  N_IF,
  N_FOR,

  N_ASSIGNMENT,
  N_ADD,
  N_SUB,
  N_MUL,
  N_DIV,
};

enum NodeFlags
{
  NF_FN_PUBLIC = 1
};

typedef struct Node
{
  int       type;
  String    raw;
  int       token_type;
  uint32_t  line;
  uint32_t  column;
  uint32_t  flags;

  struct Node*  A;
  struct Node*  B;
  struct Node*  C;
} Node;



Node*         node_alloc0         (const int type);
Node*         node_alloc1         (const int type, Node*);
Node*         node_alloc2         (const int type, Node*, Node*);
Node*         node_alloc3         (const int type, Node*, Node*, Node*);
const char*   node_type_to_string (const int type);
void          node_print_tree     (const Node*);
void          node_delete_tree    (Node*);

