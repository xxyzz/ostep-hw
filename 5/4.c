// #define _GNU_SOURCE    // for execvpe()
#include <stdio.h>
#include <stdlib.h>  // exit
#include <unistd.h>  // fork, exec*

int main() {
  int rc = fork();

  if (rc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    char *argvs[] = {"ls", "-l", "-a", "-h", NULL};
    char *envp[] = {NULL};

    execl("/bin/ls", "ls", "-l", "-a", "-h", NULL);
    execlp("ls", "ls", "-l", "-a", "-h", NULL);
    execle("/bin/ls", "ls", "-l", "-a", "-h", NULL, envp);

    execv("/bin/ls", argvs);
    execvp("ls", argvs);
    // execvpe("ls", argvs, envp);    // GNU extension
  }
  return 0;
}
