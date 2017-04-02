#pragma once
#include <cstdint>
#include "StringBuffer.h"


namespace LL0
{

  enum Tokens
  {
    T_EOF = 0,

    T_SEMICOLON,
      
    T_NUMBER,
    T_HEX_NUMBER,
    T_OCT_NUMBER,
    T_BIN_NUMBER,
    T_STRING,

    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_DOT,

    T_RPREN,
    T_LPREN
  };

  class Token
  {
  public:
    Token()
    {
      type = T_EOF;
      line = 0; column = 0;
    }

    Token(const Token&) = delete;

  public:
    Tokens        getType() const { return type; }

  protected:
    friend class Lexer;
    Tokens        type;
    uint32_t      line;
    uint32_t      column;
    StringBuffer  text;
  };

}