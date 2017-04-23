#include "alpha.h"
#include "arithmetic.h"

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

int main() {
  gen_log_tables();
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_sum),
    cmocka_unit_test(test_multiply),
    cmocka_unit_test(test_divide),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
