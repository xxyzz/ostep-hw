#include <signal.h> // kill
#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // getpid, fork, pause

int main() {
  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    printf("hello\n");
    kill(getpid(), SIGCONT);
  } else {
    pause();
    printf("goodbye\n");
  }
  return 0;
}
