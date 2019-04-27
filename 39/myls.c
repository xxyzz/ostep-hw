#include <stdio.h>     // fprintf, perror
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>    // getopt
#include <stdlib.h>    // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <dirent.h>    // opendir, readdir, closedir
#include <string.h>    // strlen, strncpy, strncmp, strncat
#include <stdbool.h>
#include <time.h>      // strftime, localtime

#define STRINGSIZE 1024
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void print_file(struct stat sb) {
    printf("%4lo  ", (unsigned long) sb.st_mode);
    printf("%3ld  ", (long) sb.st_nlink);
    printf("%3ld  %3ld  ", (long) sb.st_uid, (long) sb.st_gid);
    printf("%4lld  ", (long long) sb.st_size);
    char timeString[STRINGSIZE] = "";
    strftime(timeString, STRINGSIZE, "%b %d %H:%M", localtime(&sb.st_mtime));
    printf("%s  ", timeString);
}

int main(int argc, char *argv[]) {
    struct stat sb;
    int opt;
    char * pathname = ".";
    bool list = false;
    DIR *dp;
    opterr = 0;    // disable getopt() error message

    while ((opt = getopt(argc, argv, "l:")) != -1) {
        switch (opt) {
            case 'l':
                pathname = optarg;
                list = true;
                break;
            case '?':
                if (optopt == 'l')
                    list = true;
                break;
            default:
                break;
        }
    }

    if (!list && argc > 1)
        pathname = argv[1];
    
    if (stat(pathname, &sb) == -1)
        handle_error("stat");

    if (S_ISDIR(sb.st_mode)) {
        if ((dp = opendir(pathname)) == NULL)
            handle_error("opendir");
        struct dirent *d;
        while ((d = readdir(dp)) != NULL) {
            if (list) {
                char filePath[STRINGSIZE] = "";
                strncpy(filePath, pathname, strlen(pathname));
                strncat(filePath, "/", 1);
                strncat(filePath, d->d_name, strlen(d->d_name));
                if (stat(filePath, &sb) == -1)
                    handle_error("stat");
                print_file(sb);
            }
            printf("%s\n", d->d_name);
        }
        closedir(dp);
    } else {
        if (list)
            print_file(sb);
        printf("%s\n", pathname);
    }

    exit(EXIT_SUCCESS);
}
