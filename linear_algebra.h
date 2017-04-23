#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include <stddef.h>

unsigned char inner_product(unsigned char * a, unsigned char * b, size_t len);
void inner_product_set(unsigned char * result, unsigned char * A,
  unsigned char * x, size_t in_len, size_t out_len);

#endif
