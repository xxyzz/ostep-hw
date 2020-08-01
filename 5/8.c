#include <stdio.h>
#include <stdlib.h>   // exit
#include <string.h>   // strlen
#include <sys/wait.h> // waitpid
#include <unistd.h>   // fork, pipe, close, write

int main() {
  int pipefd[2];
  char buf;

  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid_t rc[2];
  rc[0] = fork();

  if (rc[0] < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc[0] == 0) {
    close(pipefd[0]); /* Close unused read end */
    char str[] = "The first child writes: I'm the first child.";
    write(pipefd[1], str, strlen(str));
    close(pipefd[1]);        /* Reader will see EOF */
    waitpid(rc[1], NULL, 0); /* Wait for the second child */
  } else {
    rc[1] = fork();
    if (rc[1] < 0) {
      // fork failed; exit
      fprintf(stderr, "fork failed\n");
      exit(EXIT_FAILURE);
    } else if (rc[1] == 0) {
      close(pipefd[1]); /* Close unused write end */
      // reads this string a byte at a time from the pipe and echoes it on
      // standard output.
      while (read(pipefd[0], &buf, 1) > 0) {
        write(STDOUT_FILENO, &buf, 1);
      }
      write(STDOUT_FILENO, "\n", 1);
      close(pipefd[0]);
    }
  }
  return 0;
}
