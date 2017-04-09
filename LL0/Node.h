#pragma once
#include "Utility.h"


namespace LL0
{

  enum NodeTypes
  {
    N_GLUE,
    N_EXPRESSION,

    N_N_LITERAL,
    N_IDENT,
    N_ANONYMOUS,
    N_FUNCTION,

    N_PLUS,
    N_MINUS,
    N_MUL,
    N_DIV,

    N_IF,
  };


  #define CASE(X) case X : return #X ;
  const char* nodeTypeToString(const NodeTypes type)
  {
    switch( type )
    {
      CASE(N_GLUE)
      CASE(N_EXPRESSION)
      CASE(N_N_LITERAL)
      CASE(N_IDENT)
      CASE(N_ANONYMOUS)
      CASE(N_FUNCTION)
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


  class Node
  {
  public:
    Node(const NodeTypes type, Node* A=NULL, Node* B=NULL, Node* C=NULL)
    {
      this->type  = type;
      this->A     = A;
      this->B     = B;
      this->C     = C;
    }
    ~Node()
    {
      SAFE_DELETE(A);
      SAFE_DELETE(B);
      SAFE_DELETE(C);
    }

  public:
    void print(const int index=0)
    {
      printf("%*s%s", index*2, "", nodeTypeToString(type));
      if( type==N_N_LITERAL )
        printf(" [%s] ", token.getText());
      printf("\n");
      if( A ) A->print(index+1);
      if( B ) B->print(index+1);
      if( C ) C->print(index+1);
    }

    void setToken(const Token& other)
    {
      this->token = other;
    }

  protected:
    NodeTypes type;
    Token     token;
    Node*     A;
    Node*     B;
    Node*     C;

  };


}
