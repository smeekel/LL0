#include <stdio.h>
#include <stdlib.h>
#include "InputStream.h"
#include "util.h"


typedef struct
{
  InputStream o;
  FILE* handle;
} InputStreamFile;


int is_file_readchar(PInputStream is)
{
  InputStreamFile* p = (InputStreamFile*)is;

  return fgetc(p->handle);
}

int is_file_peekchar(PInputStream is)
{
  InputStreamFile* p = (InputStreamFile*)is;
  const int chr = fgetc(p->handle);
  ungetc(chr, p->handle);
  return chr;
}

int is_file_iseof(PInputStream is)
{
  InputStreamFile* p = (InputStreamFile*)is;
  return feof(p->handle);
}

uint64_t is_file_offset(PInputStream is)
{
  InputStreamFile* p = (InputStreamFile*)is;
  return (uint64_t)_ftelli64(p->handle); //WINDOWS only
}

int is_file_close(PInputStream is)
{
  InputStreamFile* p = (InputStreamFile*)is;

  fclose(p->handle);
  free(p);

  return 0;
}

InputStream* is_openfile(const char* filename)
{
  InputStreamFile* snew = NULL;
  FILE* input = NULL;

  input = fopen(filename, "rb");
  if( !input ) return NULL;
  
  snew = (InputStreamFile*)malloc(sizeof(InputStreamFile));
  errstate_initialize(&snew->o.errors);
  snew->handle        = input;
  snew->o.close       = is_file_close;
  snew->o.readchar    = is_file_readchar;
  snew->o.peekchar    = is_file_peekchar;
  snew->o.iseof       = is_file_iseof;
  snew->o.offset      = is_file_offset;

  return (InputStream*)snew;
}
