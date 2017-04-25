#pragma once
#include "string.h"
#include "llist.h"


enum IROps
{
  NOP = 0,
  LABEL,

  PUSH,
  POP,
  MOV,
  SMOV,
  LOADK,
  CALL,
  RET,
  JMP,
  JMPF,

  ADD,
  SUB,
  MUL,
  DIV,
};

typedef struct IROp IROp;

struct IROp
{
  LListNode node;

  int     op;
  int     A;
  int     B;
  int     C;
  String  raw;
  IROp*   linkup;
};
