#pragma once
#include <stdint.h>
#include "errstate.h"

typedef struct InputStream_s* PInputStream;

typedef int       (*pf_is_close)    (PInputStream);
typedef int       (*pf_is_peekchar) (PInputStream);
typedef int       (*pf_is_readchar) (PInputStream);
typedef int       (*pf_is_eof)      (PInputStream);
typedef uint64_t  (*pf_is_offset)   (PInputStream);

typedef struct InputStream_s
{
  ErrState        errors;
  pf_is_close     close;
  pf_is_readchar  readchar;
  pf_is_peekchar  peekchar;
  pf_is_eof       iseof;
  pf_is_offset    offset;
} InputStream;


InputStream* is_openfile(const char* filename);
