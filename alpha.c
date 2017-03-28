unsigned char alpha[255];
unsigned char poly[255];

void gen_log_tables() {
  unsigned char a = 1;
  for (int i = 0; i < 255; i++) {
    alpha[i] = a;
    poly[a] = i;
    if (a & 0x80) {
      a <<= 1;
      a ^= 0x1d;
    } else {
      a <<= 1;
    }
  }
}
