#include "types.h"
#include "user.h"

int main() {
  int rc = mprotect((void *)4096, 4096);
  printf(1, "XV6_TEST_OUTPUT mprotect returned %d\n", rc);
  *(int *)4096 = 1;
  printf(1, "XV6_TEST_OUTPUT mprotect failed\n");
  exit();
}
