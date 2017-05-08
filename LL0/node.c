#include <stdio.h>
#include <malloc.h>
#include "node.h"


static void rprint(const Node* n, const int depth);


Node* node_alloc0(const int type)
{
  Node* n = (Node*)malloc(sizeof(Node));

  n->type   = type;
  n->A      = NULL;
  n->B      = NULL;
  n->C      = NULL;

  n->token_type = 0;  
  n->line       = 0;
  n->column     = 0;
  string_initialize(&n->raw);

  return n;
}

Node* node_alloc1(const int type, Node* A)
{
  Node* n = node_alloc0(type);
  n->A = A;
  n->flags = 0;
  return n;
}

Node* node_alloc2(const int type, Node* A, Node* B)
{
  Node* n = node_alloc0(type);
  n->A = A;
  n->B = B;
  n->flags = 0;
  return n;
}

Node* node_alloc3(const int type, Node* A, Node* B, Node* C)
{
  Node* n = node_alloc0(type);
  n->A = A;
  n->B = B;
  n->C = C;
  n->flags = 0;
  return n;
}

void node_delete_tree(Node* n)
{
  if( !n ) return;

  if( n->A ) node_delete_tree(n->A);
  if( n->B ) node_delete_tree(n->B);
  if( n->C ) node_delete_tree(n->C);

  string_terminate(&n->raw);

  free(n);
}

void node_print_tree(const Node* n)
{
  if( !n ) return;
  rprint(n, 0);
}


void rprint(const Node* n, const int depth)
{
  if( !n ) return;

  printf("%*s%s\n", depth*2, "", node_type_to_string(n->type));
  if( n->A ) rprint(n->A, depth+1);
  if( n->B ) rprint(n->B, depth+1);
  if( n->C ) rprint(n->C, depth+1);
}


#define CASE(X) case X : return #X ;
const char* node_type_to_string(const int type)
{
  switch( type )
  {
    CASE(N_GLUE)
    CASE(N_EXPRESSION)
    CASE(N_CALL)
    CASE(N_NEXT)

    CASE(N_N_LITERAL)
    CASE(N_IDENT)
    CASE(N_ANONYMOUS)
    CASE(N_FUNCTION)
    CASE(N_VAR)
    CASE(N_RETURN)

    CASE(N_ASSIGNMENT)

    CASE(N_ADD)
    CASE(N_SUB)
    CASE(N_MUL)
    CASE(N_DIV)
    CASE(N_IF)

    default: 
      printf("Unknown node [%d]\n", type);
      return "*UNKNOWN*";
  }
}
#undef CASE
