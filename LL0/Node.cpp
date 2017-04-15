#include "Node.h"

using namespace LL0;


#define CASE(X) case X : return #X ;
const char* LL0::nodeTypeToString(const NodeTypes type)
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

    CASE(N_PLUS)
    CASE(N_MINUS)
    CASE(N_MUL)
    CASE(N_DIV)
    CASE(N_IF)
    default: 
      printf("Unknown node [%d]\n", type);
      return "*UNKNOWN*";
  }
}
#undef CASE


Node::Node(const NodeTypes type, Node* A, Node* B, Node* C)
{
  this->type    = type;
  this->A       = A;
  this->B       = B;
  this->C       = C;

  this->line    = 0;
  this->column  = 0;
}

Node::~Node()
{
  SAFE_DELETE(A);
  SAFE_DELETE(B);
  SAFE_DELETE(C);
}

void Node::print(const int index)
{
  printf("%*s%s", index*2, "", nodeTypeToString(type));
  if( type==N_N_LITERAL )
    printf(" [%s] ", token.getText());
  printf("\n");
  if( A ) A->print(index+1);
  if( B ) B->print(index+1);
  if( C ) C->print(index+1);
}

void Node::setToken(const Token& other)
{
  this->token = other;
  setLocation(other);
}

void Node::setLocation(const Token& t)
{
  line    = t.getLine();
  column  = t.getColumn();
}
