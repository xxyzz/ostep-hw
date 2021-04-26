#include <stdio.h>  // fread, fopen, fclose
#include <stdlib.h> // exit
#include <sys/wait.h>
#include <unistd.h> // fork

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

static void write_to_file(FILE *f, char *str) { // APUE 8.9
  char *ptr;
  int c;
  for (ptr = str; (c = *ptr++) != 0;) {
    if (fputc(c, f) != c)
      errExit("fputc");
    if (fflush(f) == EOF)
      errExit("fflush");
  }
}

int main() {
  FILE *f = fopen("./2.txt", "w+");
  if (f == NULL)
    errExit("fopen");

  pid_t cpid = fork();
  if (cpid < 0)
    errExit("fork");
  else if (cpid == 0) {
    write_to_file(f, "child says hello.\n");
  } else {
    write_to_file(f, "parent says goodbye.\n");

    if (wait(NULL) == -1)
      errExit("wait");

    char buf[BUFSIZ];
    printf("file contents:\n");
    if (fseek(f, 0, 0) == -1)
      errExit("fseek");
    fread(buf, BUFSIZ, 1, f);
    printf("%s", buf);
    fclose(f);
  }
  return 0;
}
