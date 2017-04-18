#pragma once
#include <stdint.h>


typedef struct
{
  char*     buffer;
  uint32_t  offset;
  uint32_t  length;
} String;

int     string_initialize     (String*);
int     string_terminate      (String*);
int     string_clear          (String*);
int     string_push           (String*, const char);
int     string_compare        (String*, const char*);