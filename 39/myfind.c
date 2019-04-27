#include <stdio.h>     // fprintf, perror
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>    // getopt
#include <stdlib.h>    // exit
#include <string.h>    // strncmp, strlen
#include <dirent.h>    // opendir, readdir, closedir
#include <errno.h>     // EACCES
#include <limits.h>    // INT_MAX

#define STRINGSIZE 1024
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void print_dir(char * pathname, int currentDepth, int maxDepth) {
    printf("%s\n", pathname);
    DIR *dp;
    struct dirent *d;
    errno = 0;
    if (currentDepth++ > maxDepth)
        return;
    if ((dp = opendir(pathname)) == NULL) {
        if (errno == EACCES) {
            fprintf(stderr, "myfind: ‘%s‘: Permission denied\n", pathname);
            return;
        } else {
            handle_error("opendir");
        }
    }
    while ((d = readdir(dp)) != NULL) {
        char filePath[STRINGSIZE] = "";
        strncpy(filePath, pathname, strlen(pathname));
        if (strlen(filePath) != 1 || strncmp(filePath, "/", 1) != 0)
            strncat(filePath, "/", 1);
        strncat(filePath, d->d_name, strlen(d->d_name));
        if (d->d_type == DT_DIR && strncmp(d->d_name, ".", 1) != 0 && strncmp(d->d_name, "..", 2) != 0) {
            print_dir(filePath, currentDepth, maxDepth);
        } else if (strncmp(d->d_name, ".", 1) != 0 && strncmp(d->d_name, "..", 2) != 0) {
            printf("%s\n", filePath);
        }
    }
    closedir(dp);
}

int main(int argc, char *argv[]) {
    char * pathname = ".";
    struct stat sb;
    int maxDepth = INT_MAX, opt, currentDepth = 1;

    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                maxDepth = atoi(optarg);
                if (maxDepth < 0) {
                    fprintf(stderr, "Max depth must be positive.\n");
                    exit(EXIT_FAILURE);
                }
            default:
                break;
        }
    }

    if (argc > 3 && optind != 3) {
        fprintf(stderr, "Usage: %s -d [max depth] [filepath]\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    for (size_t i = optind; i < argc; i++) {
        pathname = argv[i];
    } 

    printf("pathname: %s  depth: %d\n", pathname, maxDepth);

    if (stat(pathname, &sb) == -1)
        handle_error("stat");

    if (S_ISDIR(sb.st_mode)) {
        print_dir(pathname, currentDepth, maxDepth);
    } else {
        printf("%s\n", pathname);
    }

    exit(EXIT_SUCCESS);
}
