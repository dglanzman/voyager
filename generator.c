#include "linear_algebra.h"
#include "alpha.h"
#include <stddef.h>

unsigned char generator_polynomial[33];
unsigned char generator_matrix[223*255];

static void gen_conv_matrix(unsigned char * polynomial, unsigned char * matrix,
  size_t polynomial_order, size_t output_order) {
  for (int i = 0; i < output_order - polynomial_order + 1; i++)
  for (int j = 0; j < output_order + 1; j++) {
    char tmp;
    int idx = j - i;
    if (idx < polynomial_order + 1 && idx > -1) tmp = polynomial[idx];
    else tmp = 0;
    matrix[j * (output_order - polynomial_order + 1) + i] = tmp;
  }
}

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
