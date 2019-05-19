#include <stdio.h>     // fprintf, perror
#include <stdlib.h>    // exit
#include <fcntl.h>     // open
#include <unistd.h>    // read, write, close
#include <sys/types.h>
#include "common.h"

#define BUFFSIZE 4096

int main(int argc, char *argv[]) {
    unsigned char xor = 0, buff[BUFFSIZE];
    int fdr, fdw;
    ssize_t count = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s [filepath] [checksum output file path]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fdr = open(argv[1], O_RDONLY)) == -1)
        handle_error("open");
    if ((fdw = open(argv[2], O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1)
        handle_error("open");

    while ((count = read(fdr, buff, BUFFSIZE)) != -1 && count != 0) {
        for (size_t i = 0; i < count; i++) {
            xor ^= buff[i];
        }

        if (write(fdw, &xor, 1) == -1)
            handle_error("write");
    }

    close(fdr);
    close(fdw);

    return 0;
}
