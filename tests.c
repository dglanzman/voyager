#include "alpha.h"
#include "arithmetic.h"
#include "generator.h"
#include "pgz_routines.h"

extern unsigned char message[223];
extern unsigned char codeword[255];

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

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

static void test_syndromes_1(void ** state) {
  unsigned char syn[32];
  syndromes(codeword, syn);
  for (int i = 0; i < 32; i++) {
    assert_int_equal(syn[i], 0);
  }
}

static void test_syndromes_2(void ** state) {
  unsigned char recv_word[255];
  memset(recv_word, 0, sizeof recv_word);
  recv_word[244] = alpha[0];
  recv_word[171] = alpha[0];
  recv_word[82] = alpha[0];
  recv_word[13] = alpha[0];
  for (int i = 0; i < sizeof recv_word; i++) {
    recv_word[i] = sum(recv_word[i], codeword[i]);
  }
  unsigned char synd[32];
  syndromes(recv_word, synd);
  unsigned char expected_synd[32] = {
    29, 76, 120, 157, 15, 228, 140, 95, 14, 85, 201, 186, 63,
    67, 196, 133, 11, 84, 83, 193, 85, 159, 24, 62, 165, 60,
    214, 200, 40, 206, 12, 2
  };
  for (int i = 0; i < sizeof synd; i++) {
    assert_int_equal(synd[i], expected_synd[i]);
  }
}

static void test_num_errors_1(void ** state) {
  unsigned char recv_word[255];
  memset(recv_word, 0, sizeof recv_word);
  recv_word[244] = alpha[0];
  recv_word[171] = alpha[0];
  recv_word[82] = alpha[0];
  recv_word[13] = alpha[0];
  for (int i = 0; i < sizeof recv_word; i++) {
    recv_word[i] = sum(recv_word[i], codeword[i]);
  }
  unsigned char m_synd[16 * 16];
  unsigned char synd[32];
  syndromes(recv_word, synd);
  for (int y = 0; y < 16; y++)
  for (int x = 0; x < 16; x++) {
    m_synd[y * 16 + x] = synd[x + y];
  }
  int errors = num_errors(m_synd, 16);
  assert_int_equal(errors, 4);
}

static void test_num_errors_2(void ** state) {
  unsigned char s[16];
  memset(s, 0, sizeof s);
  s[3] = 1;
  s[5] = 1;
  s[7] = 1;
  s[10] = 1;
  s[12] = 1;
  int errors = num_errors(s, 4);
  assert_int_equal(errors, 0);
}

static void test_solve(void ** state) {
  unsigned char A[16] = {
    208, 162, 245, 245,
    231, 0,   247, 124,
    33,  72,  41,  205,
    0,   140, 248, 37
  };
  unsigned char b[4] = {1, 2, 3, 4};
  unsigned char s_expected[4] = {0, 131, 7, 172};
  unsigned char s[4];
  solve(A, s, b, 4);
  for (int i = 0; i < sizeof s; i++) {
    assert_int_equal(s[i], s_expected[i]);
  }
}

static int comp(const void * a, const void * b) {
  const unsigned char * x, * y;
  x = a;
  y = b;
  return *x < *y ? -1 : *x > *y ? 1 : 0;
}

static void test_chien(void ** state) {
  unsigned char poly[5];
  unsigned char polym[5*5];
  unsigned char input[2];
  unsigned char roots[4] = {110, 52, 19, 3};
  unsigned char inv_roots[4];
  for (int i = 0; i < 4; i++) inv_roots[i] = divide(alpha[0], roots[i]);
  memset(poly, 0, sizeof poly);
  poly[0] = alpha[0];
  input[1] = alpha[0];
  for (int i = 1; i < 5; i++) {
    input[0] = inv_roots[i-1];
    gen_conv_matrix(input, polym, 1, i);
    inner_product_set(poly,  polym, poly, i, i+1);
  }
  chien_search(poly, inv_roots, sizeof inv_roots);
  qsort(inv_roots, sizeof inv_roots, 1, comp);
  qsort(roots, sizeof roots, 1, comp);
  for (int i = 0; i < 4; i++) {
    assert_int_equal(inv_roots[i], roots[i]);
  }
}

static void test_correct(void ** state) {
  unsigned char recv_word[255];
  memset(recv_word, 0, sizeof recv_word);
  recv_word[244] = alpha[0];
  recv_word[171] = alpha[0];
  recv_word[82] = alpha[0];
  recv_word[13] = alpha[0];
  for (int i = 0; i < sizeof recv_word; i++) {
    recv_word[i] = sum(recv_word[i], codeword[i]);
  }
  assert_int_equal(correct(recv_word), 4);
  for (int i = 0; i < 255; i++) {
    assert_int_equal(recv_word[i], codeword[i]);
  }
}

static void test_single_word_with_errors(void ** state) {
  unsigned char recv_word[255];
  unsigned char errors[255];
  unsigned char text[223];
  unsigned char recv_text[223];
  memset(recv_word, 0, sizeof recv_word);
  memset(text, 0, sizeof text);
  for (int i = 0; i < sizeof text / 13; i++) {
    memcpy(text + i * 13, "Hello World! ", 13);
  }
  inner_product_set(recv_word, generator_matrix, text, 223, 255);
  errors[244] = alpha[0];
  errors[171] = alpha[0];
  errors[82] = alpha[0];
  errors[13] = alpha[0];
  for (int i = 0; i < sizeof recv_word; i++) {
    recv_word[i] = sum(recv_word[i], errors[i]);
  }
  assert_int_equal(correct(recv_word), 4);
  divide_polynomial(recv_word, generator_polynomial, recv_text, NULL, 255, 33);
  for (int i = 0; i < 233; i++) {
    assert_int_equal(recv_text[i], text[i]);
  }
}

int main() {
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
    cmocka_unit_test(test_syndromes_1),
    cmocka_unit_test(test_syndromes_2),
    cmocka_unit_test(test_num_errors_1),
    cmocka_unit_test(test_num_errors_2),
    cmocka_unit_test(test_solve),
    cmocka_unit_test(test_chien),
    cmocka_unit_test(test_correct),
    cmocka_unit_test(test_single_word_with_errors),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
