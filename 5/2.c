#include <fcntl.h>    // open
#include <stdio.h>    // fgetc, fopen, fclose
#include <stdlib.h>   // exit
#include <string.h>   // strlen
#include <sys/stat.h> // S_IRWXU
#include <sys/wait.h>
#include <unistd.h> // fork, write, close

int main() {
  int fd = open("./2.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
  int rc = fork();
  write(fd, "First line.\n", strlen("First line.\n"));

  if (rc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    write(fd, "child writes a line.\n", strlen("child writes a line.\n"));
    printf("file descriptor in child process: %d\n", fd);
  } else {
    write(fd, "parent writes a line.\n", strlen("parent writes a line.\n"));
    printf("file descriptor in parent prosess: %d\n", fd);

    if (wait(NULL) == -1) {
      fprintf(stderr, "wait failed\n");
      exit(EXIT_FAILURE);
    }
    FILE *fp;
    fp = fopen("./2.txt", "r");
    int ch;
    printf("\nfile contents: \n");
    while ((ch = fgetc(fp)) != EOF)
      printf("%c", ch);
    fclose(fp);
  }

  close(fd);
  return 0;
}
