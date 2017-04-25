#include <getopt.h>
#include <stddef.h> // for size_t
#include <stdio.h>
#include <errno.h>
#include <string.h> // for memset
#include "generator.h"
#include "linear_algebra.h"

int main(int argc, char * argv[]) {
  init_generator();
  int c, error = 0;
  char * ifile = NULL, * ofile = NULL;
  while ((c = getopt(argc, argv, "f:o:")) != -1) {
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
    fprintf(stderr, "Usage: encoder [ -f infile ] [ -o outfile ]\n");
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
  for (;;) {
    size_t len = fread(message, 1, sizeof message, in);
    if (len != 223) {
      if (errno) { // error condition
        perror("reading input file");
        if (ifile) fclose(in);
        if (ofile) fclose(out);
        return 2;
      } else { // message length not multiple of 223, pad with 0
        memset(message + len, 0, 223 - len);
        done = 1;
      }
    }

    inner_product_set(codeword, generator_matrix, message, sizeof message,
      sizeof codeword);

    fwrite(codeword, 1, sizeof codeword, out);
    if (errno) {
      perror("writing output file");
      if (ifile) fclose(in);
      if (ofile) fclose(out);
      return 2;
    }
    if (done) break;
  }

}
