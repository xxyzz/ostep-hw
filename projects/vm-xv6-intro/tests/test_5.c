#include "types.h"
#include "user.h"

int main() {
  int rc = mprotect((void *)4096, 4096);
  printf(1, "XV6_TEST_OUTPUT mprotect returned %d\n", rc);
  rc = munprotect((void *)4096, 4096);
  printf(1, "XV6_TEST_OUTPUT munprotect returned %d\n", rc);
  printf(1, "XV6_TEST_OUTPUT address 4096: %d\n", *(int *)4096);
  *(int *)4096 = 1;
  printf(1, "XV6_TEST_OUTPUT address 4096: %d\n", *(int *)4096);
  exit();
}
