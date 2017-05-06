#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stddef.h>

unsigned char sum(unsigned char x, unsigned char y);
unsigned char multiply(unsigned char x, unsigned char y);
unsigned char divide(unsigned char x, unsigned char y);
unsigned char inner_product(unsigned char * a, unsigned char * b,
  size_t len);
void inner_product_set(unsigned char * result, unsigned char * A,
  unsigned char * x, size_t in_len, size_t out_len);
void gen_conv_matrix(unsigned char * polynomial, unsigned char * matrix,
  size_t polynomial_order, size_t output_order);
void divide_polynomial(unsigned char * dividend, unsigned char * divisor,
  unsigned char * quotient, unsigned char * remainder, size_t dividend_len,
  size_t divisor_len);

#endif
