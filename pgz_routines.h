#ifndef PGZ_ROUTINES_H
#define PGZ_ROUTINES_H
#include <stdlib.h>

void syndromes(unsigned char * recv_word, unsigned char * synd);
int num_errors(unsigned char * syn_mtx, size_t len);

#endif
