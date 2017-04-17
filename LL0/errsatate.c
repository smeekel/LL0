#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include "errstate.h"
#include "util.h"


int errstate_initialize(PError p)
{
  p->message  = NULL;
  p->line     = 0;
  return SUCCESS;
}

int errstate_terminate(PError p)
{
  SAFE_FREE(p->message);
  return SUCCESS;
}

int errstate_adderror(PError p, const char* format, ...)
{
  va_list args;

  SAFE_FREE(p->message);

  va_start(args, format);
  const int size = _vsnprintf(NULL, 0, format, args);
  va_end(args);

  p->message = (char*)malloc(size+1);
  if( !p->message ) return E_MALLOC;

  va_start(args, format);
  _vsnprintf(p->message, size, format, args);
  va_end(args);

  p->message[size] = 0;

  return SUCCESS;
}

int errstate_haserror(PError p)
{
  return (p->message!=NULL);
}
