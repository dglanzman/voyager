#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "alpha.h"
#include "arithmetic.h"
#include "pgz_routines.h"
#include "generator.h"

int main(int argc, char * argv[]) {
  init_generator();
  int c, error = 0;
  char * ifile = NULL, * ofile = NULL;
  while((c = getopt(argc, argv, "f:o:")) != -1) {
    switch (c) {
    case 'f':
      ifile = optarg;
    break;
    case 'o':
      ofile = optarg;
    break;
    case ':':
      fprintf(stderr, "Option -%c requires an operand\n", optopt);
      error = 1;
    break;
    case '?':
      fprintf(stderr, "Unrecognized option: -%c\n", optopt);
      error = 1;
    }
  }
  if (error) {
    fprintf(stderr, "Usage: decoder [ -f infile] [ -o outfile ]\n");
    return 1;
  }
  FILE * in = stdin, * out = stdout;
  if (ifile) {
    in = fopen(ifile, "r");
    if (!in) {
      perror("input file");
      return 2;
    }
  }
  if (ofile) {
    out = fopen(ofile, "w");
    if (!out) {
      perror("output file");
      if (ifile) fclose(in);
      return 2;
    }
  }

  char message[223], codeword[255];
  errno = 0;
  int done = 0;
  size_t len = fread(codeword, 1, sizeof codeword, in);
  if (len != 255) {
    if (errno) {
      perror("reading input file");
      if (ifile) fclose(in);
      if (ofile) fclose(out);
      return 2;
    } else if (len != 0) {
      fprintf(stderr, "Encoded message not multiple of 255 bytes, aborting.");
      if (ifile) fclose(in);
      if (ofile) fclose(out);
      return 1;
    } else {
      if (ifile) fclose(in);
      if (ofile) fclose(out);
      return 0;
    }
  }
  for (;;) {
    
    int errors = correct(codeword);
    for (int i = 0; i < errors; i++) fputc('.', stderr);
    divide_polynomial(codeword, generator_polynomial, message, NULL, 255, 33);
    len = fread(codeword, 1, sizeof codeword, in);
    if (errno) {
      perror("reading input file");
      if (ifile) fclose(in);
      if (ofile) fclose(out);
      return 2;
    } else if (len != 255 && len != 0) {
      fprintf(stderr, "Encoded message not multiple of 255 bytes, aborting.");
      if (ifile) fclose(in);
      if (ofile) fclose(out);
      return 1;
    } else if (len == 0) {
      done = 1;
      len = sizeof message;
      while (len > 0 && !message[len-1]) len--;
    } else {
      len = sizeof message;
    }
    fwrite(message, 1, len, out);
    if (errno) {
      perror("writing output file");
      if (ifile) fclose(in);
      if (ofile) fclose(out);
      return 2;
    }
    if (done) break;
  }
}
