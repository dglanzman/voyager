#include "alpha.h"
#include "arithmetic.h"
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

static void test_eval_at_alpha(void ** state) {
  assert_int_equal(inner_product(codeword, alpha, sizeof alpha), 0);
}

static void test_polydiv_1(void ** state) {
  unsigned char p1[] = {4, 1, 0, 1};
  unsigned char p2[] = {1, 1};
  unsigned char q[3];
  unsigned char r[1];
  divide_polynomial(p1, p2, q, r, sizeof p1, sizeof p2);
  assert_int_equal(q[0], 0);
  assert_int_equal(q[1], 1);
  assert_int_equal(q[2], 1);
  assert_int_equal(r[0], 4);
}

static void test_polydiv_2(void ** state) {
  unsigned char p1[] = {30, 21, 99, 3};
  unsigned char p2[] = {250};
  unsigned char q[4];
  unsigned char q_expect[] = {127, 73, 241, 37};
  unsigned char * r = NULL;
  divide_polynomial(p1, p2, q, r, sizeof p1, sizeof p2);
  for (int i = 0; i < sizeof q; i++)
    assert_int_equal(q[i], q_expect[i]);
}

static void test_polydiv_3(void ** state) {
  unsigned char p1[] = {30, 21};
  unsigned char p2[] = {5, 5, 5, 5};
  unsigned char r[2];
  unsigned char r_expect[] = {30, 21};
  unsigned char * q = NULL;
  divide_polynomial(p1, p2, q, r, sizeof p1, sizeof p2);
  for (int i = 0; i < sizeof r; i++)
    assert_int_equal(r[i], r_expect[i]);
}

int main() {
  gen_log_tables();
  init_generator();
  for (int i = 0; i < sizeof message / 2; i++) {
    unsigned char tmp = message[i];
    message[i] = message[sizeof message - 1 - i];
    message[sizeof message - 1 - i] = tmp;
  }
  for (int i = 0; i < sizeof codeword / 2; i++) {
    unsigned char tmp = codeword[i];
    codeword[i] = codeword[sizeof codeword - 1 - i];
    codeword[sizeof codeword - 1 - i] = tmp;
  }
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_sum),
    cmocka_unit_test(test_multiply),
    cmocka_unit_test(test_divide),
    cmocka_unit_test(test_genpoly),
    cmocka_unit_test(test_encode_1),
    cmocka_unit_test(test_eval_at_alpha),
    cmocka_unit_test(test_polydiv_1),
    cmocka_unit_test(test_polydiv_2),
    cmocka_unit_test(test_polydiv_3),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
