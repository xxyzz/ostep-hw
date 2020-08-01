#include <signal.h>  // kill
#include <stdio.h>
#include <stdlib.h>  // exit
#include <unistd.h>  // getpid, fork, pause

static void sigcont_handler() { printf("goodbye\n"); }

int main() {
  int parent_pid = getpid();
  int rc = fork();
  struct sigaction act;
  act.sa_handler = sigcont_handler;

  if (rc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    kill(parent_pid, SIGCONT);
    printf("hello\n");
  } else {
    sigaction(SIGCONT, &act, NULL);
    pause();
  }
  return 0;
}
