#include <stdio.h>
#include <stdlib.h>      // exit, malloc
#include <sys/stat.h>    // fstat
#include <string.h>      // strcpy, strcmp

typedef struct linkedList
{
    char *line;
    struct linkedList *next;
} LinkedList;

int
main(int argc, char *argv[])
{
    FILE *in = NULL, *out = NULL;
    in  = stdin;
    out = stdout;

    if (argc == 2) {
        if ((in = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(EXIT_FAILURE);
        }
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

        struct stat *s1 = malloc(sizeof(struct stat)), *s2 = malloc(sizeof(struct stat));
        if (s1 == NULL || s2 == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(EXIT_FAILURE);
        }

        if (fstat(fileno(in), s1) != 0) {
            free(s1);
            free(s2);
            fprintf(stderr, "reverse: fstat error\n");
            exit(EXIT_FAILURE);
        }
        if (fstat(fileno(out), s2) != 0) {
            free(s1);
            free(s2);
            fprintf(stderr, "reverse: fstat error\n");
            exit(EXIT_FAILURE);
        }

        if (s1->st_ino == s2->st_ino) {
            free(s1);
            free(s2);
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(EXIT_FAILURE);
        }

        free(s1);
        free(s2);
    }

    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(EXIT_FAILURE);
    }

    LinkedList *head = malloc(sizeof(LinkedList)), *curr = NULL, *prev = NULL;

    if (head == NULL) {
        fprintf(stderr, "reverse: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    head->line = NULL;
    head->next = NULL;
    curr = head;
    char *line = NULL;
    size_t linecap = 0;

    // store lines to list
    while (getline(&line, &linecap, in) > 0) {
        if (head->line == NULL) {
            if ((head->line = malloc(linecap)) == NULL) {
                fprintf(stderr, "reverse: malloc failed\n");
                exit(EXIT_FAILURE);
            }
            strcpy(head->line, line);
        } else {
            LinkedList *temp = malloc(sizeof(LinkedList));
            if (temp == NULL) {
                fprintf(stderr, "reverse: malloc failed\n");
                exit(EXIT_FAILURE);
            }
            if ((temp->line = malloc(linecap)) == NULL) {
                fprintf(stderr, "reverse: malloc failed\n");
                exit(EXIT_FAILURE);
            }
            strcpy(temp->line, line);

            curr->next = temp;
            curr = temp;
        }
    }

    // reverse list
    curr = head;
    while (curr != NULL) {
        LinkedList *temp = curr->next;
        curr->next = prev;
        prev = curr;
        curr = temp;
    }
    head = prev;

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