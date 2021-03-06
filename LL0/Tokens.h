#pragma once


enum Tokens
{
  T_ERROR = -1,
  T_EOF   = 0,

  T_SEMICOLON,
      
  T_NUMBER,
  T_HEX_NUMBER,
  T_OCT_NUMBER,
  T_BIN_NUMBER,
  T_STRING,
  T_IDENT,

  T_PLUS,
  T_MINUS,
  T_MUL,
  T_DIV,
  T_MOD,
  T_AND,
  T_OR,
  T_NOT,
  T_B_AND,
  T_B_OR,
  T_B_XOR,
  T_B_NOT,
  T_LSHIFT,
  T_RSHIFT,

  T_ASSIGNMENT,
  T_EQ,
  T_NEQ,
  T_GT,
  T_LT,
  T_GTE,
  T_LTE,
    
  T_DOT,
  T_COMMA,
  T_LPREN,
  T_RPREN,
  T_LBRACE,
  T_RBRACE,

  T_IF,
  T_ELSE,
  T_RETURN,
  T_FOR,
  T_WHILE,
  T_FUNCTION,
  T_VAR,
  T_IMPORT,
  T_AS,
  T_PUBLIC,
};

