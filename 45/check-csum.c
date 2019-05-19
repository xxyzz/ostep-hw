#include <stdio.h>     // fprintf, perror
#include <stdlib.h>    // exit
#include <fcntl.h>     // open
#include <unistd.h>    // read, write, close
#include "common.h"

#define BUFFSIZE 4096

int main(int argc, char *argv[]) {
    unsigned char xor = 0, buff_a[BUFFSIZE], buff_b = 0;
    int fda, fdb;
    ssize_t count = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s [filepath] [checksum output file path]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fda = open(argv[1], O_RDONLY)) == -1)
        handle_error("open");
    if ((fdb = open(argv[2], O_RDONLY)) == -1)
        handle_error("open");

    while ((count = read(fda, buff_a, BUFFSIZE)) != -1 && count != 0) {
        for (size_t i = 0; i < count; i++) {
            xor ^= buff_a[i];
        }

        if ((count = read(fdb, &buff_b, 1)) == -1)
            handle_error("read");
        if (xor != buff_b) {
            printf("The file is corrupted!\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("The file is fine.\n");
    close(fda);
    close(fdb);

    return 0;
}
