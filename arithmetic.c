#include "alpha.h"
#include "arithmetic.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

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

unsigned char divide(unsigned char x, unsigned char y) {
  unsigned char xa, ya, za;
  if (!y) abort();
  if (!x) return 0;
  xa = poly[x];
  ya = poly[y];
  if ((int)xa - ya < 0) {
    za = (int)xa - ya + 255;
  } else {
    za = xa - ya;
  }
  return alpha[za];
}

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

void gen_conv_matrix(unsigned char * polynomial, unsigned char * matrix,
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

void divide_polynomial(unsigned char * dividend, unsigned char * divisor,
  unsigned char * quotient, unsigned char * remainder, size_t dividend_len,
  size_t divisor_len) {
  if (divisor_len > dividend_len) {
    for (int i = 0; i < dividend_len; i++) {
      remainder[i] = dividend[i];
    }
    return;
  }
  unsigned char * quotient_divisor_product = malloc(dividend_len);
  if (!quotient_divisor_product) abort(); 
  unsigned char * product_dividend_difference = malloc(dividend_len);
  if (!product_dividend_difference) abort(); 
  unsigned char * divisor_conv = malloc(divisor_len * dividend_len);
  if (!divisor_conv) abort();
  size_t quotient_len = dividend_len - (divisor_len - 1);
  memset(quotient, 0, quotient_len);
  gen_conv_matrix(divisor, divisor_conv, divisor_len - 1, dividend_len - 1);

  for (;;) {
    inner_product_set(quotient_divisor_product, divisor_conv, quotient,
      quotient_len, dividend_len);
    for (int i = 0; i < dividend_len; i++) {
      product_dividend_difference[i] = sum(dividend[i],
        quotient_divisor_product[i]);
    }
    int diff_greatest_nonzero = dividend_len;
    for (;;) {
      diff_greatest_nonzero--;
      if (diff_greatest_nonzero == -1) break;
      if (product_dividend_difference[diff_greatest_nonzero]) break;
    }
    if (diff_greatest_nonzero < divisor_len - 1) {
      for (int i = 0; i <= diff_greatest_nonzero; i++) {
        remainder[i] = product_dividend_difference[i];
      }
      return;
    }
    unsigned char factor = divide(product_dividend_difference[diff_greatest_nonzero],
      dividend[dividend_len - 1]);
    quotient[diff_greatest_nonzero - dividend_len - 1] = factor;
  }
}
