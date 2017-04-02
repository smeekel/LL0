#pragma once
#include <stdint.h>


namespace LL0
{

  class StringBuffer
  {
  public:
    StringBuffer();
    StringBuffer(const StringBuffer&) = delete;
    ~StringBuffer();

  public:
    StringBuffer& operator=(const StringBuffer&);
    operator const char* () const { return ptr; }

    void  clear   ();
    void  push    (const char c);

  protected:
    char*     ptr;
    uint64_t  length;
    uint64_t  offset;
  };

}
