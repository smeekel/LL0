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
    void        clear     ();
    void        push      (const char c);
    bool        compare   (const char* string) const;
    
    const char* toString  () const { return ptr; }

    StringBuffer& operator=             (const StringBuffer&);
                  operator const char*  () const { return toString(); }

  protected:
    char*     ptr;
    uint64_t  length;
    uint64_t  offset;
  };

}
