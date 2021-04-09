#include <stdio.h>    // getline, fileno, fopen, fclose, fprintf
#include <stdlib.h>   // exit, malloc
#include <string.h>   // strdup
#include <sys/stat.h> // fstat
#include <sys/types.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    fprintf(stderr, msg);                                                      \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

typedef struct linkedList {
  char *line;
  struct linkedList *next;
} LinkedList;

int main(int argc, char *argv[]) {
  FILE *in = NULL, *out = NULL;
  in = stdin;
  out = stdout;

  if (argc == 2 && (in = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  if (argc == 3) {
    if ((in = fopen(argv[1], "r")) == NULL) {
      fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
      exit(EXIT_FAILURE);
    }
    if ((out = fopen(argv[2], "w")) == NULL) {
      fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
      exit(EXIT_FAILURE);
    }

    struct stat sb1, sb2;
    if (fstat(fileno(in), &sb1) == -1 || fstat(fileno(out), &sb2) == -1)
      handle_error("reverse: fstat error\n");
    if (sb1.st_ino == sb2.st_ino)
      handle_error("reverse: input and output file must differ\n");
  } else if (argc > 3)
    handle_error("usage: reverse <input> <output>\n");

  LinkedList *head = NULL;
  char *line = NULL;
  size_t len = 0;
  // store lines to list in reverse order
  while (getline(&line, &len, in) != -1) {
    LinkedList *node = malloc(sizeof(LinkedList));
    if (node == NULL) {
      free(line);
      handle_error("reverse: malloc failed\n");
    }
    if ((node->line = strdup(line)) == NULL) {
      free(line);
      handle_error("reverse: strdup failed\n");
    }
    node->next = head;
    head = node;
  }

  // print reversed lines
  while (head != NULL) {
    LinkedList *temp = head;
    fprintf(out, "%s", head->line);
    head = head->next;
    free(temp->line);
    free(temp);
  }

  free(line);
  fclose(in);
  fclose(out);
  return 0;
}
