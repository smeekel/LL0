#include "Parser.h"
#include "IStream.h"
#include "Exception.h"

using namespace LL0;




bool Parser::lexReadNext()
{
  c = input->readChar();
  columnNumber++;
  return (c==0);
}

Parser::Tokens Parser::lexNext()
{
  tokenRaw.clear();

  if (input->isEof())
    return Tokens::T_EOF;


lex_start:
  switch( c )
  {
    case '\t': case '\v':
    case ' ':  case '\r':
    {
      lexReadNext();
      goto lex_start;
    }

    case '\n':
    {
      lexNewLine();
      lexReadNext();
      goto lex_start;
    }

    case '/':
    {
      const char peek = input->peekChar();

      if( peek=='/' || peek=='*' )
      {
        // Single or multi line column
        lexEatComment();
        goto lex_start;
      }
      else
      {
        lexReadNext();
        return T_DIV;
      }
    }

    case '+': lexReadNext(); return T_PLUS;
    case '-': lexReadNext(); return T_MINUS;
    case '*': lexReadNext(); return T_MUL;

    case '(': lexReadNext(); return T_LPREN;
    case ')': lexReadNext(); return T_RPREN;
    case ';': lexReadNext(); return T_SEMICOLON;
    case '.': lexReadNext(); return T_DOT;
    case -1: return T_EOF;

    default:
    {
      if( isDigit(c) )
        return lexReadNumber();

      throw EXCEPTION("Lexer: unexpected input at %d:%d", lineNumber, columnNumber);
    }
  }

}

void Parser::lexNewLine()
{
  lineNumber++;
  columnNumber = 0;
}

Parser::Tokens Parser::lexReadNumber()
{
  //
  // Base 10: 0-9+
  // Base 16: 0-9A-Fa-f+
  // Base 8:  0-7+
  // Base 2:  0b0-1+
  // Float:   0-9+\.0-9+
  // 
  // Note:
  // Beats me if the above it valid regex. I never tested it.
  //
  const bool startWithZero  = (c=='0');
        bool hasDecimal     = false;

  if( startWithZero )
  {
    const char peek = input->peekChar();
    if( peek=='b' )
    {
      //
      // Binary number
      //
      lexReadNext(); //eat 'b'
      lexReadNext(); //ready next digit
      lexBinaryNumber();
      return T_BIN_NUMBER;
    }
    else if( peek=='x' )
    {
      //
      // Base 16
      //
      lexReadNext(); //eat 'x'
      lexReadNext(); //ready next digit
      lexHexNumber();
      return T_HEX_NUMBER;
    }
  }


  //
  // Start paring number
  //
  int largestDigit = 0;
  while( true )
  {
    tokenRaw.push(c);

    if( c>largestDigit ) largestDigit = c;

    lexReadNext();
    if( c=='.' )
    {
      const char peek = input->peekChar();
      if( !hasDecimal )
      {
        if( !isDigit(peek) )
          break;
        hasDecimal = true;
      }
      else
      {
        break;
      }
    }
    else if( !isDigit(c) )
    {
      break;
    }
  }


  if( startWithZero )
  {
    if( largestDigit>'7' )
      throw EXCEPTION("(%d:%d) Invalid digit in octal number", lineNumber, columnNumber);
    return T_OCT_NUMBER;
  }

  return T_NUMBER;
}

void Parser::lexBinaryNumber()
{
  int digitCount = 0;

  while( true )
  {
    if( c!='0' && c!='1' )
    {
      if( !isDigit(c) && digitCount>0 )
        break;
      else
        throw EXCEPTION("(%d:%d) Invalid digit in binary number", lineNumber, columnNumber);
    }

    digitCount++; //NOTE: possible overflow condition
    tokenRaw.push(c);
    lexReadNext();
  }
}

void Parser::lexHexNumber()
{
  int digitCount = 0;

  while( true )
  {
    if( !isHexDigit(c) )
    {
      if( digitCount>0 && !isAlpha(c) )
        break;
      else
        throw EXCEPTION("(%d:%d) Invalid digit in hex number", lineNumber, columnNumber);
    }

    digitCount++; //NOTE: possible overflow condition
    tokenRaw.push(c);
    lexReadNext();
  }
}

void Parser::lexEatComment()
{
  lexReadNext();

  if( c=='*' )
  {
    // Multi-line comment
    while( true )
    {
      lexReadNext();
      if( c=='*' && input->peekChar()=='/' )
      {
        lexReadNext();
        lexReadNext();
        return;
      }
      else if( c=='\n' )
      {
        
        lexNewLine();
      }
      else if( input->isEof() )
      {
        throw EXCEPTION("Unexpected EOF in comment");
      }
    }
  }
  else
  {
    // Single-line comment
    while( true )
    {
      lexReadNext();
      if( c=='\n' )
      {
        lexNewLine();
        lexReadNext();
        return;
      }
      else if( input->isEof() )
      {
        return;
      }
    }
  }
}
