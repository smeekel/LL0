#pragma once


enum IROps
{
  NOP = 0,
  MOV,
};

typedef struct
{
  int op;
  int A;
  int B;
  int C;
} IROp;
