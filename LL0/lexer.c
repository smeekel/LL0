#include "lexer.h"
#include "tokens.h"


#define PEEKCHAR()  (p->input->peekchar(p->input))
#define C           (p->c)

static int  next                (Lexer*);
static int  nextchar            (Lexer*);
static int  eat_comment         (Lexer*);
static void newline             (Lexer*);
static int  read_number         (Lexer*);
static int  read_literal        (Lexer*);
static int  read_hex_number     (Lexer*);
static int  read_binary_number  (Lexer*);
static bool is_digit            (const int);
static bool is_alpha            (const int);
static bool is_literal          (const int);
static bool is_hex              (const int);



int lexer_initialize(Lexer* p, const char* filename)
{
  errstate_initialize(&p->error);
  string_initialize(&p->raw);
  string_initialize(&p->current_raw);

  p->c = 0;

  p->token          = T_ERROR;
  p->current_token  = T_ERROR;
  p->line           = 1;
  p->column         = 0;
  p->current_line   = 1;
  p->current_column = 0;

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

int lexer_terminate(Lexer* p)
{
  string_terminate  (&p->current_raw);
  string_terminate  (&p->raw);
  errstate_terminate(&p->error);
  return SUCCESS;
}

int lexer_next(Lexer* p)
{
  p->current_token  = p->token;
  p->current_line   = p->line;
  p->current_column = p->column;
  string_copy(&p->raw, &p->current_raw);

  if( p->input->iseof(p->input) )
    p->token = T_EOF;
  else
    p->token = next(p);

  return p->current_token;
}


void newline(Lexer* p)
{
  p->line++;
  p->column = 0;
}

int nextchar(Lexer* p)
{
  p->c = p->input->readchar(p->input);
  p->column++;
  return p->c;
}

int next(Lexer* p)
{
  string_clear(&p->raw);

  if( p->input->iseof(p->input) )
    return T_EOF;

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
        if( !eat_comment(p) ) return T_ERROR;
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
      else if( is_literal(p->c) )
        return read_literal(p);

      errstate_adderror(&p->error, "[%d:%d] Unexpected input", p->line, p->column);
      return T_ERROR;
    }
  }
}

int read_literal(Lexer* p)
{
  string_push(&p->raw, C);
  nextchar(p);

  while( is_literal(C) )
  {
    string_push(&p->raw, C);
    nextchar(p);
  }

  #define COMPARE(x) ( string_compare_cstr(&p->raw, (x))==0 )
  
  int type = T_IDENT;
  switch( p->raw.buffer[0] )
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
    string_clear(&p->raw);


  return type;
}

int read_number(Lexer* p)
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
      nextchar(p);
      nextchar(p);
      return read_binary_number(p);
    }
    else if( peek=='x' )
    {
      nextchar(p);
      nextchar(p);
      return read_hex_number(p);
    }
  }


  int largestDigit = 0;
  while( true )
  {
    string_push(&p->raw, C);

    if( C>largestDigit ) largestDigit = C;

    nextchar(p);
    if( C=='.' )
    {
      const char peek = PEEKCHAR();
      if( hasDecimal ) break;

      if( !is_digit(peek) )
        break;
      hasDecimal = true;
    }
    else if( !is_digit(C) )
    {
      break;
    }
  }

  if( startsWithZero )
  {
    if( largestDigit>'7' )
    {
      errstate_adderror(&p->error, "[%d:%d] Invalid digit in octal number", p->line, p->column);
      return T_ERROR;
    }
    return T_OCT_NUMBER;
  }
  
  return T_NUMBER;
}

int read_binary_number(Lexer* p)
{
  int digitCount = 0;

  while( true )
  {
    if( C!='0' && C!='1' )
    {
      if( is_digit(C) && digitCount>0 )
        break;
      errstate_adderror(&p->error, "[%d:%d] Invalid digit in binary number", p->line, p->column);
      return T_ERROR;
    }

    string_push(&p->raw, C);
    digitCount++;
    nextchar(p);
  }

  return T_BIN_NUMBER;
}

int read_hex_number(Lexer* p)
{
  int digitCount = 0;

  while( true )
  {
    if( !is_hex(C) )
    {
      if( digitCount>0 && !is_alpha(C) )
        break;
      errstate_adderror(&p->error, "[%d:%d] Invalid digit in hex number", p->line, p->column);
      return T_ERROR;
    }

    string_push(&p->raw, C);
    digitCount++;
    nextchar(p);
  }

  return T_HEX_NUMBER;
}

int eat_comment(Lexer* p)
{
  nextchar(p);

  if( C=='*' )
  {
    //
    // Multi-line
    //
    while( true )
    {
      nextchar(p);
      if( C=='*' && PEEKCHAR()=='/' )
      {
        nextchar(p);
        nextchar(p);
        return SUCCESS;
      }
      else if( C=='\n' )
      {
        newline(p);
      }
      else if( p->input->iseof(p->input) )
      {
        errstate_adderror(&p->error, "[%d:%d] Unexpected EOF in comment", p->line, p->column);
        return T_ERROR;
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
      nextchar(p);
      if( C=='\n' )
      {
        newline(p);
        nextchar(p);
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

bool is_literal(const int c)
{
  return is_alpha(c) || is_digit(c) || c=='$' || c=='_';
}
