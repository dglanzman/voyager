#include "arithmetic.h"
#include "alpha.h"
#include <stddef.h>

unsigned char generator_polynomial[33];
unsigned char generator_matrix[223*255];

void init_generator() {
  unsigned char conv_matrix[33*33];
  unsigned char input[2];
  gen_log_tables();
  generator_polynomial[0] = alpha[0];
  input[1] = alpha[0];
  for (int i = 1; i < 33; i++) {
    input[0] = alpha[i];
    gen_conv_matrix(input, conv_matrix, 1, i);
    inner_product_set(generator_polynomial, conv_matrix,
      generator_polynomial, i, i+1);
  }
  gen_conv_matrix(generator_polynomial, generator_matrix, 32, 254);
}
