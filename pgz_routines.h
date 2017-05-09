#ifndef PGZ_ROUTINES_H
#define PGZ_ROUTINES_H
#include <stdlib.h>

void syndromes(unsigned char * recv_word, unsigned char * synd);
int num_errors(unsigned char * syn_mtx, size_t len);
void solve(unsigned char * A, unsigned char * sol, unsigned char * b,
  size_t len);
void chien_search(unsigned char * factors, unsigned char * locations,
  size_t nu);
int correct(unsigned char * recv_word);

#endif
