#pragma once
#include <cstdint>
#include <memory>
#include "StringBuffer.h"
#include "exception.h"
#include "Tokens.h"


namespace LL0
{

  using SmartToken = std::shared_ptr<Token>;

  class Lexer
  {
  public:
    Lexer(class IStream* input);
    ~Lexer();

  public:
    SmartToken  next();

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
    Tokens  lexReadLiteral  ();

  private:
    class IStream*  input;
    int             c;
    uint32_t        lineNumber;
    uint32_t        columnNumber;
    StringBuffer    tokenRaw;

  };

}
