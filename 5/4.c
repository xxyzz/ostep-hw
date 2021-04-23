#define _GNU_SOURCE // for execvpe()
#if __has_include(<features.h>)
#include <features.h>
#endif
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h> // fork, exec*

int main() {
  char *argvs[] = {"ls", "-l", "-a", "-h", NULL};
  char *envp[] = {NULL};
  int rc = fork();

  if (rc == 0) {
    printf("execl:\n");
    execl("/bin/ls", "ls", "-l", "-a", "-h", NULL);
  } else {
    wait(NULL);
    rc = fork();
    if (rc == 0) {
      printf("\nexeclp:\n");
      execlp("ls", "ls", "-l", "-a", "-h", NULL);
    } else {
      wait(NULL);
      rc = fork();
      if (rc == 0) {
        printf("\nexecle:\n");
        execle("/bin/ls", "ls", "-l", "-a", "-h", NULL, envp);
      } else {
        wait(NULL);
        rc = fork();
        if (rc == 0) {
          printf("\nexecv:\n");
          execv("/bin/ls", argvs);
        } else {
          wait(NULL);
          rc = fork();
          if (rc == 0) {
            printf("\nexecvp:\n");
            execvp("ls", argvs);
          } else {
            wait(NULL);
            rc = fork();
            if (rc == 0) {
              printf("\nexecve:\n");
              execve("/bin/ls", argvs, envp);
            } else {
              wait(NULL);
#ifdef __GNU_LIBRARY__
              rc = fork();
              if (rc == 0) {
                printf("\nexecvpe:\n");
                execvpe("ls", argvs, envp);
              } else
                wait(NULL);
#endif
            }
          }
        }
      }
    }
  }

  return 0;
}
