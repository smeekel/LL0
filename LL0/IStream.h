#pragma once
#include <stdint.h>


namespace LL0
{

  class IStream
  {
  public:
    explicit IStream() {}
    virtual ~IStream() {}

  public:
    virtual int       readChar    () = 0;
    virtual int       peekChar    () = 0;
    virtual bool      isEof       () = 0;
    virtual uint64_t  getOffset   () = 0;
  };

}
