#pragma once

typedef struct
{
  char*         message;
  unsigned int  line;
} ErrState;

typedef ErrState* PError;

int errstate_initialize (PError);
int errstate_terminate  (PError);
int errstate_adderror   (PError, const char* format, ...);
int errstate_haserror   (PError);

#define SUCCESS 0
#define ERROR   -1
#define E_MALLOC -2
