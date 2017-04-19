#pragma once
#include <stdint.h>


typedef struct
{
  char*     buffer;
  uint32_t  offset;
  uint32_t  length;
} String;

void    string_initialize     (String*);
void    string_terminate      (String*);
void    string_clear          (String*);
int     string_push           (String*, const char);
int     string_compare_cstr   (String*, const char*);
int     string_copy           (const String*, String*);
int     string_copy_cstr      (String*, const char*);
