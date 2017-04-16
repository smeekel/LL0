#pragma once

typedef struct
{
  const char*   message;
  unsigned int  line;
} ErrState;

typedef ErrState* PError;

int errstate_Initialize (PError);
int errstate_Terminate  (PError);

