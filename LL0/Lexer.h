#pragma once
#include <cstdint>
#include "StringBuffer.h"
#include "exception.h"
#include "Tokens.h"


namespace LL0
{

  class Lexer
  {
  public:
    Lexer(class IStream* input);
    ~Lexer();

  public:
    Token*  next();

  public:
    static bool  isDigit    (const char c);
    static bool  isHexDigit (const char c);
    static bool  isAlpha    (const char c);

  protected:
    Tokens  lexNext         ();
    bool    lexReadNext     ();
    void    lexEatComment   ();
    void    lexNewLine      ();
    Tokens  lexReadNumber   ();
    void    lexBinaryNumber ();
    void    lexHexNumber    ();

  private:
    class IStream*  input;
    int             c;
    uint32_t        lineNumber;
    uint32_t        columnNumber;
    StringBuffer    tokenRaw;

  };

}
