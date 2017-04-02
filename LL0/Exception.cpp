#define _CRT_SECURE_NO_WARNINGS
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "Exception.h"


using namespace LL0;

Exception::Exception()
{
  message[0]  = 0;
  filename[0] = 0;
}

Exception& Exception::setMessage(const char* format, ...)
{
  va_list args;

  va_start(args, format);
    std::vsnprintf(message, sizeof(message)-1, format, args);
  va_end(args);

  message[sizeof(message)-1] = 0;

  return *this;
}

Exception& Exception::setFilename(const char* filename)
{
  std::strncpy(this->filename, filename, sizeof(this->filename)-1);
  this->filename[sizeof(this->filename) - 1] = 0;
  return *this;
}

Exception& Exception::setLinenumber(const uint32_t linenumber)
{
  this->linenumber = linenumber;
  return *this;
}


