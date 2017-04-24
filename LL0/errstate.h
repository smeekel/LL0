#pragma once

typedef struct
{
  char*         message;
  unsigned int  line;
} ErrState;

typedef ErrState* PError;

int           errstate_initialize (PError);
int           errstate_terminate  (PError);
int           errstate_adderror   (PError, const char* format, ...);
int           errstate_haserror   (PError);
const char*   errstate_getmessage (PError);
void          errstate_clear_all  (PError);


#define SUCCESS   1
#define ERROR     0
#define E_MALLOC -1
