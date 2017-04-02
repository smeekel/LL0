#define _CRT_SECURE_NO_WARNINGS
#include "IStreamFile.h"
#include "Exception.h"

using namespace LL0;


IStreamFile::IStreamFile(const char* filename)
{
  if( !filename )
    throw EXCEPTION("filename must not be null");

  handle = std::fopen(filename, "rb");
  if( !handle )
    throw EXCEPTION("Could not open file for reading");
}

IStreamFile::~IStreamFile()
{
  std::fclose(handle);
  handle = NULL;
}

int IStreamFile::readChar()
{
  return std::fgetc(handle);
}

int IStreamFile::peekChar()
{
  const char c = std::fgetc(handle);
  std::ungetc(c, handle);
  return c;
}

bool IStreamFile::isEof()
{
  return std::feof(handle);
}

uint64_t IStreamFile::getOffset()
{
  return _ftelli64(handle);
}


