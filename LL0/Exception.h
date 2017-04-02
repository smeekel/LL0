#pragma once
#include <cstdint>


namespace LL0
{
  
  class Exception
  {
  public:
    Exception();

  public:
    Exception&  setMessage    (const char* format, ...);
    Exception&  setFilename   (const char* filename);
    Exception&  setLinenumber (const uint32_t linenumber);
    const char* getMessage    () const { return message; }
    const char* getFilename   () const { return filename; }
    uint32_t    getLinenumber () const { return linenumber; }

  private:
    char      message [255];
    char      filename[32];
    uint32_t  linenumber;
  };

  #define EXCEPTION(format, ...) Exception().setFilename(__FILE__).setLinenumber(__LINE__).setMessage(format, __VA_ARGS__)
}
