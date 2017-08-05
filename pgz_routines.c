#include "alpha.h"
#include "arithmetic.h"
#include <string.h>
#include <stdlib.h>
#include "pgz_routines.h"

static unsigned char poly_eval(unsigned char * poly, size_t len, unsigned char at) {
  if (len == 1) return *poly;
  unsigned char * x = malloc(len);
  if (!x) abort();
  unsigned char at_copy = at;
  x[0] = 1;
  for (int i = 1; i < len; i++) {
    x[i] = at;
    at = multiply(at, at_copy);
  }
  unsigned char ret = inner_product(poly, x, len);
  free(x);
  return ret;
}

void syndromes(unsigned char * recv_word, unsigned char * synd) {
  for (int s = 0; s < 32; s++) {
    synd[s] = poly_eval(recv_word, 255, alpha[s+1]);
  }
}

static void swap_rows(unsigned char * matrix, size_t len, int row1, int row2) {
  unsigned char tmp;
  for (int i = 0; i < len; i++) {
    tmp = matrix[row1 * len + i];
    matrix[row1 * len + i] = matrix[row2 * len + i];
    matrix[row2 * len + i] = tmp;
  }
}

static void norm_row(unsigned char * row, size_t len) {
  int state = 0;
  unsigned char lead_factor = 0;
  for (int i = 0; i < len; i++) {
    if (state == 0) {
      if (!row[i]) continue;
      lead_factor = row[i];
      row[i] = alpha[0];
      state++;
    } else if (state == 1) {
      row[i] = divide(row[i], lead_factor);
    }
  }
}

static void kill_row(unsigned char * ref, unsigned char * dest, size_t len) {
  int state = 0;
  unsigned char lead_factor = 0;
  for (int i = 0; i < len; i++) {
    if (state == 0) {
      if (!ref[i]) continue;
      lead_factor = dest[i];
      dest[i] = 0;
      state++;
    } else if (state == 1) {
      dest[i] = sum(multiply(ref[i], lead_factor), dest[i]);
    }
  }
}

static void sort_rows(unsigned char * m, size_t len) {
  size_t init = 0;
  for (int x = 0; x < len; x++) {
    int uy = init;
    int ly = len - 1;
    int last_uy = uy;
    for (;;) {
      while (uy < len && m[uy * len + x]) uy++;
      while (ly > -1 && !m[ly * len + x]) ly--;
      if (ly > uy) {
        swap_rows(m, len, uy, ly);
        last_uy = uy + 1;
      }
      else break;
    }
    init = last_uy;
  }
}

static void upper_triangle(unsigned char * matrix, size_t len) {
  sort_rows(matrix, len);
  for (int y = 0; y < len; y++) {
    int x = 0;
    while (!matrix[y*len + x]) {
      x++;
      if (x >= len) return;
    }
    unsigned char lead_factor = matrix[y*len + x];
    for (int xd = x; xd < len; xd++) {
      matrix[y*len + xd] = divide(matrix[y*len + xd], lead_factor);
    }
    for (int yd = y + 1; yd < len; yd++) {
      lead_factor = matrix[yd*len + x];
      if (!lead_factor) continue;
      for (int xd = x; xd < len; xd++) {
        unsigned char cell = matrix[yd*len + xd];
        cell = divide(cell, lead_factor);
        cell = sum(cell, matrix[y*len + xd]);
        matrix[yd*len + xd] = cell;
      }
    }
  }
}

static int singular(unsigned char * mtx, size_t len) {
  unsigned char * copy = malloc(len*len);
  if (!copy) abort();
  memcpy(copy, mtx, len*len);
  upper_triangle(copy, len);
  int ret = 0;
  for (int i = 0; i < len*len; i += len + 1) {
    if (!copy[i]) {
      ret = 1;
      break;
    }
  }
  free(copy);
  return ret;
}

int num_errors(unsigned char * syn_mtx, size_t len) {
  for (;;) {
    if (singular(syn_mtx, len)) {
      int i = 0;
      for (int y = 0; y < len-1; y++)
      for (int x = 0; x < len-1; x++) {
        int j = y * len + x;
        syn_mtx[i] = syn_mtx[j];
        i++;
      }
      len--;
    } else {
      return len;
    }
  }
}

void solve(unsigned char * A, unsigned char * sol, unsigned char * b,
  size_t len) {
  unsigned char * m = malloc(len*(len+1));
  if (!m) abort();
  for (int y = 0; y < len; y++)
  for (int x = 0; x < len; x++)
    m[y * (len+1) + x] = A[y * len + x];
  for (int i = 0; i < len; i++) {
    m[i * (len+1) + len] = b[i];
  }
  memset(sol, 0, len);
  for (int x = 0; x < len+1; x++) {
    int found_row = -1;
    for (int y = 0; y < len; y++) {
      if (m[y * (len+1) + x] && !sol[y]) {
        found_row = y;
        sol[y] = 1;
        break;
      }
    }
    if (found_row == -1) continue;
    norm_row(&m[found_row * (len+1)], len+1);
    for (int y = 0; y < len; y++) {
      if (!m[y * (len+1) + x]) continue;
      else if (y == found_row) continue;
      kill_row(&m[found_row * (len+1)], &m[y * (len+1)], len+1);
    }
  }
  for (int y = 0; y < len; y++)
  for (int x = 0; x < len; x++) {
    if (m[y * (len+1) + x]) sol[x] = m[y * (len+1) + len];
  }
  free(m);
}

void chien_search(unsigned char * factors, unsigned char * locations,
  size_t nu) {
  int idx = 0;
  for (int i = 1; i < 256; i++) {
    if (!poly_eval(factors, nu+1, i)) {
      locations[idx] = divide(alpha[0], i);
      idx++;
    }
  }
}

int correct(unsigned char * recv_word) {
  unsigned char s[32];
  unsigned char sm[16*16];
  unsigned char locators[16];
  unsigned char lm[16*16];
  unsigned char errors[16];
  unsigned char loc_poly[17];
  syndromes(recv_word, s);
  for (int y = 0; y < 16; y++)
  for (int x = 0; x < 16; x++) {
    sm[16 * y + x] = s[x + y];
  }
  int len = num_errors(sm, 16);
  if (!len) return 0;
  solve(sm, locators, s+len, len);
  loc_poly[0] = alpha[0];
  for (int i = 1; i < len+1; i++) loc_poly[i] = locators[len - i];
  chien_search(loc_poly, locators, len);
  for (int x = 0; x < len; x++) lm[x] = locators[x];
  for (int y = 1; y < len; y++)
  for (int x = 0; x < len; x++) {
    lm[y * len + x] = multiply(locators[x], lm[(y-1) * len + x]);
  }
  solve(lm, errors, s, len);
  for (int i = 0; i < len; i++) {
    recv_word[poly[locators[i]]] = sum(recv_word[poly[locators[i]]], errors[i]);
  }
}
