#ifndef FIXED_FLAOTING_POINT
#define FIXED_FLAOTING_POINT

#define FF_q 16
#define FF_f (1 << FF_q)

int CONVERT_FF (int n);

int CONVERT_DOWN (int x);

int CONVERT_NEAREST (int x);

int ADD_FF (int x, int y);

int SUB_FF (int x, int y);

int ADD_FI (int x, int n);

int SUB_FI (int x, int n);

int MUL_FF (int x, int y);

int MUL_FI (int x, int n);

int DIV_FF (int x, int y);

int DIV_FI (int x, int n);

#endif