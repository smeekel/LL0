#pragma once
#include <cstdio>
#include "IStream.h"


namespace LL0
{

  class IStreamFile : public IStream
  {
  public:
    explicit IStreamFile(const char* filename);
    virtual ~IStreamFile();

  public:
    virtual int       readChar();
    virtual int       peekChar();
    virtual bool      isEof();
    virtual uint64_t  getOffset();

  private:
    FILE* handle;
  };

}
