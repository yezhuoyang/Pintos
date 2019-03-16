#include "lib/fixed_floating_point.h"
#include "lib/stdint.h"

int CONVERT_FF (int n)
{
  return n * FF_f;
}

int CONVERT_DOWN (int x)
{
  return x / FF_f;
} 

int CONVERT_NEAREST (int x)
{
  if (x >= 0)
    return x + FF_f / 2;
  else
    return x - FF_f / 2;
}

int ADD_FF (int x, int y)
{
  return x + y;
}

int SUB_FF (int x, int y)
{
  return x - y;
}

int ADD_FI (int x, int n)
{
  return x + n * FF_f;
}

int SUB_FI (int x, int n)
{
  return x - n * FF_f;
}

int MUL_FF (int x, int y)
{
  return ((int64_t) x) * y / FF_f;
}

int MUL_FI (int x, int n)
{
  return x * n;
}

int DIV_FF (int x, int y)
{
  return ((int64_t) x) * FF_f / y;
}

int DIV_FI (int x, int n)
{
  return x / n;
}
