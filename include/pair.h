#pragma once

#include "bool.h"

struct PAIR_INT {
  int first;
  int second;
};
typedef struct PAIR_INT pair_int;

void pair_int_print(pair_int);

void pair_int_print(pair_int pi) {
  printf("(%d,%d)", pi.first, pi.second);
}
