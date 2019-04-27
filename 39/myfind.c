#include <stdio.h>     // fprintf, perror
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>    // exit
#include <string.h>    // strncmp, strlen
#include <dirent.h>    // opendir, readdir, closedir
#include <errno.h>     // EACCES

#define STRINGSIZE 1024
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void print_dir(char * pathname) {
    printf("%s\n", pathname);
    DIR *dp;
    struct dirent *d;
    errno = 0;
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
        if (strncmp(pathname, "/", 1) != 0)
            strncat(filePath, "/", 1);
        strncat(filePath, d->d_name, strlen(d->d_name));
        if (d->d_type == DT_DIR && strncmp(d->d_name, ".", 1) != 0 && strncmp(d->d_name, "..", 2) != 0) {
            print_dir(filePath);
        } else if (strncmp(d->d_name, ".", 1) != 0 && strncmp(d->d_name, "..", 2) != 0) {
            printf("%s\n", filePath);
        }
    }
    closedir(dp);
}

int main(int argc, char *argv[]) {
    char * pathname = ".";
    struct stat sb;
    if (argc >= 2)
        pathname = argv[1];

    if (stat(pathname, &sb) == -1)
        handle_error("stat");

    if (S_ISDIR(sb.st_mode)) {
        print_dir(pathname);
    } else {
        printf("%s\n", pathname);
    }

    exit(EXIT_SUCCESS);
}
