#pragma once
#include "Utility.h"


namespace LL0
{

  enum NodeTypes
  {
    N_GLUE,
    N_EXPRESSION,

    N_N_LITERAL,

    N_PLUS,
    N_MINUS,
    N_MUL,
    N_DIV,
  };


  #define CASE(X) case X : return #X ;
  const char* nodeTypeToString(const NodeTypes type)
  {
    switch( type )
    {
      CASE(N_GLUE)
      CASE(N_EXPRESSION)
      CASE(N_N_LITERAL)
      CASE(N_PLUS)
      CASE(N_MINUS)
      CASE(N_MUL)
      CASE(N_DIV)
      default: return "*UNKNOWN*";
    }
  }
  #undef CASE


  class Node
  {
  public:
    Node(const NodeTypes type)
    {
      this->type  = type;
      this->A     = NULL;
      this->B     = NULL;
    }
    Node(const NodeTypes type, Node* A, Node* B=NULL)
    {
      this->type  = type;
      this->A     = A;
      this->B     = B;
    }
    ~Node()
    {
      SAFE_DELETE(A);
      SAFE_DELETE(B);
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

  };


}
