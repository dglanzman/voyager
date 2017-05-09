#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "alpha.h"
#include "arithmetic.h"
#include "pgz_routines.h"

int main(int argc, char * argv[]) {
  gen_log_tables();
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

  char message[223], codeword[225];
  errno = 0;
  int done = 0;
  for (;;) {
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
        done = 1;
      }
    }
    
    int errors = correct(codeword);
    for (int i = 0; i < errors; i++) fputc('.', stderr);
    decode(codeword, message);
    len = sizeof message;
    if (done) {
      while (len > 0 && !message[len-1]) len--;
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
