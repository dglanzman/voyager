#include "alpha.h"

unsigned char sum(unsigned char x, unsigned char y) {
  return x ^ y;
}

unsigned char multiply(unsigned char x, unsigned char y) {
  unsigned char xa, ya, za;
  if (!x || !y) return 0;
  xa = poly[x];
  ya = poly[y];
  if ((int)xa + ya > 254) {
    za = (int)xa + ya - 255;
  } else {
    za = xa + ya;
  }
  return alpha[za];
}
