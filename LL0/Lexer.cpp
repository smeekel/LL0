#include "Lexer.h"
#include "IStream.h"
#include "Exception.h"
#include "Utility.h"

using namespace LL0;



Lexer::Lexer(IStream* input)
{
  if( !input )
    throw EXCEPTION("input must not be null");

  this->input         = input;
  this->lineNumber    = 1;
  this->columnNumber  = 0;
  
  lexReadNext();  // prime the input stream
  next();         // prime the token look ahead
}

Lexer::~Lexer()
{
  SAFE_DELETE(input);
}


SmartToken Lexer::next()
{
  const Tokens t = lexNext();

  SmartToken toReturn = peekToken;
  SmartToken token(new Token());

  token->type   = t;
  token->line   = lineNumber;
  token->column = columnNumber;
  token->text   = tokenRaw;
  peekToken = token;

  return toReturn;
}

SmartToken Lexer::peek()
{
  return peekToken;
}

bool Lexer::lexReadNext()
{
  c = input->readChar();
  columnNumber++;
  return (c==0);
}

Tokens Lexer::lexNext()
{
  tokenRaw.clear();

  if (input->isEof())
    return Tokens::T_EOF;


lex_start:
  switch( c )
  {
    case '\t': case '\v':
    case ' ':  case '\r':
    case '\x0C': //form feed
    case '\x85': //next line
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

    #define SINGLE(x, t) case x: lexReadNext(); return t ;
    SINGLE('+', T_PLUS);
    SINGLE('-', T_MINUS);
    SINGLE('*', T_MUL);
    SINGLE('%', T_MOD);
    SINGLE('(', T_LPREN);
    SINGLE(')', T_RPREN);
    SINGLE('{', T_LBRACE);
    SINGLE('}', T_RBRACE);
    SINGLE(';', T_SEMICOLON);
    SINGLE('.', T_DOT);
    SINGLE(',', T_COMMA);
    SINGLE('=', T_ASSIGNMENT);
    SINGLE(-1, T_EOF);
    #undef SINGLE

    default:
    {
      if( isDigit(c) )
        return lexReadNumber();
      if( isAlpha(c) )
        return lexReadLiteral();

      throw EXCEPTION("Lexer: unexpected input at %d:%d", lineNumber, columnNumber);
    }
  }
}

Tokens Lexer::lexReadLiteral()
{
  tokenRaw.push(c);
  lexReadNext();

  while( isAlpha(c) || isDigit(c) || c=='_' )
  {
    tokenRaw.push(c);
    lexReadNext();
  }

  #define COMPARE(x)  (tokenRaw.compare(x))
  const char first = tokenRaw.toString()[0];
  Tokens type = T_IDENT;
  
  switch( first )
  {
    case 'e':
    {
      if( COMPARE("else") )
        type = T_ELSE;
      break;
    }

    case 'f':
    {
      if( COMPARE("false") )
        type = T_FALSE;
      else if( COMPARE("for") )
        type = T_FOR;
      else if( COMPARE("fn") )
        type = T_FUNCTION;
      break;
    }

    case 'i':
    {
      if( COMPARE("if") )
        type = T_IF;
      else if( COMPARE("import") )
        type = T_IMPORT;
      else if( COMPARE("into") )
        type = T_INTO;
      break;
    }

    case 'r':
    {
      if( COMPARE("return") )
        type = T_RETURN;
      break;
    }

    case 't':
    {
      if( COMPARE("true") )
        type = T_TRUE;
       break;
    }

    case 'v':
    {
      if( COMPARE("var") )
        type = T_VAR;
      break;
    }

    case 'w':
    {
      if( COMPARE("while") )
        type = T_WHILE;
      break;
    }

    default: break;
  }

  if( type!=T_IDENT )
    tokenRaw.clear();

  return type;
}

void Lexer::lexNewLine()
{
  lineNumber++;
  columnNumber = 0;
}

Tokens Lexer::lexReadNumber()
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

void Lexer::lexBinaryNumber()
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

void Lexer::lexHexNumber()
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

void Lexer::lexEatComment()
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

bool Lexer::isDigit(const char c)
{
  return c>='0' && c<='9' ;
}

bool Lexer::isHexDigit(const char c)
{
  return 
    ( c>='0' && c<='9' )
    ||
    ( c>='A' && c<='F' )
    || 
    ( c>='a' && c<='f' )
    ;
}

bool Lexer::isAlpha(const char c)
{
  return 
    ( c>='A' && c<='Z' )
    || 
    ( c>='a' && c<='z' )
    ;
}
