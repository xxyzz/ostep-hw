#include "types.h"
#include "user.h"

int main() {
  char *n = 0;
  int rc = 0;
  rc = write(1, n, 4);
  printf(1, "XV6_TEST_OUTPUT %d\n", rc);
  exit();
}
