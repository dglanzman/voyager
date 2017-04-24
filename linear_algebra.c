#include "arithmetic.h"
#include "linear_algebra.h"
#include <stdlib.h>
#include <string.h>

unsigned char inner_product(unsigned char * a, unsigned char * b, size_t len) {
  unsigned char result = 0;
  for (int i = 0; i < len; i++) {
    result = sum(multiply(a[i], b[i]), result);
  }
  return result;
}

void inner_product_set(unsigned char * result, unsigned char * A,
  unsigned char * x, size_t in_len, size_t out_len) {
  unsigned char * x_copy = malloc(in_len);
  if (!x_copy) abort();
  memcpy(x_copy, x, in_len);
  for (int i = 0; i < out_len; i++) {
    unsigned char tmp = 0;
    for (int j = 0; j < in_len; j++) {
      tmp = sum(multiply(A[in_len * i + j], x_copy[j]), tmp);
    }
    result[i] = tmp;
  }
  free(x_copy);
}
