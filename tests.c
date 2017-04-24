#include "alpha.h"
#include "arithmetic.h"
#include "linear_algebra.h"
#include "generator.h"

extern unsigned char message[223];
extern unsigned char codeword[255];

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

static void test_sum(void ** state) {
  unsigned char result;
  result = sum(1, 1);
  assert_int_equal(result, 0);
  result = sum(2, 1);
  assert_int_equal(result, 3);
  result = sum(5, 2);
  assert_int_equal(result, 7);
  result = sum(7, 4);
  assert_int_equal(result, 3);
}

static void test_multiply(void ** state) {
  unsigned char result;
  result = multiply(1, 1);
  assert_int_equal(result, 1);
  result = multiply(5, 5);
  assert_int_equal(result, 17);
  result = multiply(100, 9);
  assert_int_equal(result, 99);
  result = multiply(254, 254);
  assert_int_equal(result, 227);
  result = multiply(0, 4);
  assert_int_equal(result, 0);
  result = multiply(128, 2);
  assert_int_equal(result, 29);
}

static void test_divide(void ** state) {
  unsigned char result;
  result = divide(1, 1);
  assert_int_equal(result, 1);
  result = divide(5, 5);
  assert_int_equal(result, 1);
  result = divide(100, 9);
  assert_int_equal(result, 144);
  result = divide(2, 128);
  assert_int_equal(result, 54);
  result = divide(0, 4);
  assert_int_equal(result, 0);
  result = divide(128, 2);
  assert_int_equal(result, 64);
}

static void test_genpoly(void ** state) {
  unsigned char res[] = 
    {45, 216, 239, 24, 253, 104, 27, 40,
     107, 50, 163, 210, 227, 134, 224, 158,
     119, 13, 158, 1, 238, 164, 82, 43, 15,
     232, 246, 142, 50, 189, 29, 232, 1};

  for (int i = 0; i < sizeof res / 2; i++) {
    unsigned char tmp = res[i];
    res[i] = res[sizeof res - 1 - i];
    res[sizeof res - 1 - i] = tmp;
  }

  for (int i = 0; i < sizeof res; i++) {
    assert_int_equal(generator_polynomial[i], res[i]);
  }
}

static void test_encode_1(void ** state) {
  unsigned char computed_codeword[255];
  inner_product_set(computed_codeword, generator_matrix, message, 223, 255);
  for (int i = 0; i < sizeof computed_codeword; i++) {
    assert_int_equal(computed_codeword[i], codeword[i]);
  }
}

int main() {
  init_generator();
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_sum),
    cmocka_unit_test(test_multiply),
    cmocka_unit_test(test_divide),
    cmocka_unit_test(test_genpoly),
    cmocka_unit_test(test_encode_1),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
