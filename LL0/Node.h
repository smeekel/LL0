#pragma once
#include <memory>
#include <cstdint>
#include "Utility.h"
#include "Tokens.h"


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
    N_VAR,
    N_RETURN,

    N_ASSIGNMENT,

    N_PLUS,
    N_MINUS,
    N_MUL,
    N_DIV,

    N_IF,
  };


  const char* nodeTypeToString(const NodeTypes type);

  class Node
  {
  public:
    Node(const NodeTypes type, Node* A=NULL, Node* B=NULL, Node* C=NULL);
    ~Node();

  public:
    void print      (const int index=0);
    void setToken   (const Token&);
    void setLocation(const Token&);

    bool        is        (NodeTypes compareType) const { return type==compareType; }
    NodeTypes   getType   () const { return type; }
    const char* getRaw    () const { return token.getText(); }
    const Node* getA      () const { return A; }
    const Node* getB      () const { return B; }
    const Node* getC      () const { return C; }
    uint32_t    getLine   () const { return line; }
    uint32_t    getColumn () const { return column; }

  protected:
    NodeTypes type;
    Token     token;
    Node*     A;
    Node*     B;
    Node*     C;

    uint32_t  line;
    uint32_t  column;
  };

  using SafeNode = std::shared_ptr<Node>;

}
