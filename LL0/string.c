#include <string.h>
#include <malloc.h>
#include "string.h"
#include "errstate.h" 
#include "util.h"


static int copy(String* dst, const char* src, const size_t size);


void string_initialize(String* s)
{
  s->buffer = NULL;
  s->offset = 0;
  s->length = 0;
}

void string_terminate(String* s)
{
  SAFE_FREE(s->buffer);
}

void string_clear(String* s)
{
  s->offset = 0;
  if( s->length>0 ) s->buffer[0] = 0;
}

int string_compare_cstr(const String* s, const char* B)
{
  if( s->buffer==NULL )
  {
    return ( B==NULL ) ? 0 : -1 ;
  }
  else
  {
    if( B==NULL ) return 1;
    return strcmp(s->buffer, B);
  }
}

int string_compare(const String* A, const String* B)
{
  return string_compare_cstr(A, B->buffer);
}

int string_push(String* s, const char C)
{
  if( s->offset+1>s->length )
  {
    s->buffer = (char*)realloc(s->buffer, (size_t)s->length+32);
    s->length += 32;
  }

  s->buffer[s->offset++] = C;
  s->buffer[s->offset]   = 0;

  return SUCCESS;
}

int string_copy(const String* src, String* dst)
{
  copy(dst, src->buffer, src->offset);
  dst->offset = src->offset;

  return SUCCESS;
}

int string_copy_cstr(String* dst, const char* src)
{
  const size_t length = strlen(src);
  copy(dst, src, length);
  dst->offset = (uint32_t)length;

  return SUCCESS;
}

int copy(String* dst, const char* src, const size_t size)
{
  if( size>0 && dst->length<size+1 )
  {
    dst->buffer = (char*)realloc(dst->buffer, size+1);
    dst->length = (uint32_t)(size+1);
  }

  //dst->offset = size;
  if( size>0 )
  {
    strncpy(dst->buffer, src, size);
    dst->buffer[size] = 0;
  }
  else if( dst->length>0 )
  {
    dst->buffer[0] = 0;
  }

  return SUCCESS;
}