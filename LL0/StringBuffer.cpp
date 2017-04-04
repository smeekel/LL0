#include <memory>
#include <cstring>
#include "StringBuffer.h"

using namespace LL0;


StringBuffer::StringBuffer()
{
  ptr     = NULL;
  length  = 0;
  offset  = 0;
}

StringBuffer::~StringBuffer()
{
  if( ptr ) std::free(ptr);
  ptr = NULL;
}

void StringBuffer::clear()
{
  offset = 0;
  if( ptr ) *ptr = 0;
}

void StringBuffer::push(const char c)
{
  if( offset+1>=length )
  {
    ptr = (char*)std::realloc(ptr, (size_t)length+128);
    length+=128;
  }

  ptr[offset++] = c;
  ptr[offset]   = 0;
}

StringBuffer& StringBuffer::operator=(const StringBuffer& other)
{
  if( ptr ) std::free(ptr);
  ptr = NULL;

  if( other.ptr )
  {
    ptr = (char*)std::malloc( (size_t)(other.offset+1) );
    memcpy(ptr, other.ptr, (size_t)other.offset);
    ptr[other.offset] = 0;

    offset  = other.offset;
    length  = offset + 1;
  }
  else
  {
    offset = length = 0;
  }

  return *this;
}

bool StringBuffer::compare(const char* string) const
{
  if( ptr==NULL )
    return (string==NULL);
  if( string==NULL )
    return false;

  return std::strcmp(ptr, string)==0 ;
}
