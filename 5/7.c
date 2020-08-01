#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // fork

int main() {
  pid_t rc = fork();
  if (rc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    close(STDOUT_FILENO);
    printf("child calls printf().");
  }
  return 0;
}
