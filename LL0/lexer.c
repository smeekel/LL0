#include "lexer.h"
#include "tokens.h"


#define READCHAR()  (p->input->readchar(p->input))
#define PEEKCHAR()  (p->input->peekchar(p->input))
#define C           (p->c)

static int  nextchar            (LexerState*);
static int  eat_comment         (LexerState*);
static void newline             (LexerState*);
static int  read_number         (LexerState*);
static int  read_literal        (LexerState*);
static int  read_hex_number     (LexerState*);
static int  read_binary_number  (LexerState*);
static bool is_digit            (const int);
static bool is_alpha            (const int);
static bool is_hex              (const int);



int lexer_initialize(LexerState* p, const char* filename)
{
  errstate_initialize(&p->error);
  string_initialize(&p->tokenraw);

  p->c      = 0;
  p->line   = 1;
  p->column = 0;

  p->input = is_openfile(filename);

  if( !p->input )
  {
    errstate_adderror(&p->error, "Failed to open lexer input file");
    return ERROR;
  }

  nextchar(p);    // prime input stream
  lexer_next(p);  // prime look-ahead

  return SUCCESS;
}

int lexer_terminate(LexerState* p)
{
  string_terminate  (&p->tokenraw);
  errstate_terminate(&p->error);
  return SUCCESS;
}


void newline(LexerState* p)
{
  p->line++;
  p->column = 0;
}

int nextchar(LexerState* p)
{
  p->c = READCHAR();
  p->column++;
  return (p->input==0);
}

int lexer_next(LexerState* p)
{
  string_clear(&p->tokenraw);

  if( p->input->iseof(p->input) )
    return 0;

restart:
  switch( p->c )
  {
    case '\t':    case '\v':
    case '\r':    case ' ':
    case '\x0C':  case '\x85':
      nextchar(p);
      goto restart;

    case '\n':
      newline(p);
      nextchar(p);
      goto restart;

    case '/':
    {
      const char peek = PEEKCHAR();
      if( peek=='/' || peek=='*' )
      {
        if( !eat_comment(p) ) return ERROR;
        goto restart;
      }
      else
      {
        nextchar(p);
        return T_DIV;
      }
    }

    #define SINGLE(x, t) case x: nextchar(p); return t ;
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
      if( is_digit(p->c) )
        return read_number(p);
      else if( is_alpha(p->c) )
        return read_literal(p);

      errstate_adderror(&p->error, "[%d:%d] Unexpected input", p->line, p->column);
      return ERROR;
    }
  }
}

int read_literal(LexerState* p)
{
  string_push(&p->tokenraw, C);
  READCHAR();

  while( is_alpha(C) || is_digit(C) || C=='_' )
  {
    string_push(&p->tokenraw, C);
    READCHAR();
  }

  #define COMPARE(x) ( string_compare(&p->tokenraw, (x)) )
  
  int type = T_IDENT;
  switch( '0' )
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
    string_clear(&p->tokenraw);


  return type;
}

int read_number(LexerState* p)
{
  //
  // Base 10: 0-9+
  // Base 16: 0-9A-Fa-f+
  // Base 8:  0-7+
  // Base 2:  0b0-1+
  // Float:   0-9+\.0-9+
  //
  const bool  startsWithZero  = (C=='0');
        bool  hasDecimal      = false;

  if( startsWithZero )
  {
    const int peek = PEEKCHAR();
    if( peek=='b' )
    {
      READCHAR();
      READCHAR();
      return read_binary_number(p);
    }
    else if( peek=='x' )
    {
      READCHAR();
      READCHAR();
      return read_hex_number(p);
    }
  }


  int largestDigit = 0;
  while( true )
  {
    string_push(&p->tokenraw, C);

    if( C>largestDigit ) largestDigit = C;

    READCHAR();
    if( C=='.' )
    {
      const char peek = PEEKCHAR();
      if( hasDecimal ) break;

      if( !is_digit(peek) )
        break;
      hasDecimal = true;
    }
    else if( is_digit(C) )
    {
      break;
    }
  }

  if( startsWithZero )
  {
    if( largestDigit>'7' )
    {
      errstate_adderror(&p->error, "[%d:%d] Invalid digit in octal number", p->line, p->column);
      return ERROR;
    }
    return T_OCT_NUMBER;
  }
  
  return T_NUMBER;
}

int read_binary_number(LexerState* p)
{
  int digitCount = 0;

  while( true )
  {
    if( C!='0' && C!='1' )
    {
      if( is_digit(C) && digitCount>0 )
        break;
      errstate_adderror(&p->error, "[%d:%d] Invalid digit in binary number", p->line, p->column);
      return ERROR;
    }

    string_push(&p->tokenraw, C);
    digitCount++;
    READCHAR();
  }

  return T_BIN_NUMBER;
}

int read_hex_number(LexerState* p)
{
  int digitCount = 0;

  while( true )
  {
    if( !is_hex(C) )
    {
      if( digitCount>0 && !is_alpha(C) )
        break;
      errstate_adderror(&p->error, "[%d:%d] Invalid digit in hex number", p->line, p->column);
      return ERROR;
    }

    string_push(&p->tokenraw, C);
    digitCount++;
    READCHAR();
  }

  return T_HEX_NUMBER;
}

int eat_comment(LexerState* p)
{
  READCHAR();

  if( C=='*' )
  {
    //
    // Multi-line
    //
    while( true )
    {
      READCHAR();
      if( C=='*' && PEEKCHAR()=='/' )
      {
        READCHAR();
        READCHAR();
        return SUCCESS;
      }
      else if( C=='\n' )
      {
        newline(p);
      }
      else if( p->input->iseof(p->input) )
      {
        errstate_adderror(&p->error, "[%d:%d] Unexpected EOF in comment", p->line, p->column);
        return ERROR;
      }
    }
  }
  else
  {
    //
    // Single-line
    //
    while( true )
    {
      READCHAR();
      if( C=='\n' )
      {
        newline(p);
        READCHAR();
        return SUCCESS;
      }
      else if( p->input->iseof(p->input) )
      {
        return SUCCESS;
      }
    }
  }
}


bool is_digit(const int c)
{
  return c>='0' && c<='9' ;
}

bool is_hex(const int c)
{
  return 
    ( c>='0' && c<='9' )
    ||
    ( c>='A' && c<='F' )
    || 
    ( c>='a' && c<='f' )
    ;
}

bool is_alpha(const int c)
{
  return 
    ( c>='A' && c<='Z' )
    || 
    ( c>='a' && c<='z' )
    ;
}

