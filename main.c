#include "arithmetic.h"
#include "alpha.h"
#include <stdio.h>

int main() {
  gen_log_tables();
  printf("testing some arithmetic ops...\n");
  printf("a^100 = 0x%x * a^20 = 0x%x == %x\n",
    alpha[100], alpha[20], multiply(alpha[100], alpha[20]));
}
