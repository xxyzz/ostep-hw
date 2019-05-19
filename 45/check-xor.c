#include <stdio.h>     // fprintf, perror
#include <stdlib.h>    // exit
#include <fcntl.h>     // open
#include <unistd.h>    // read
#include "common.h"

int main(int argc, char *argv[]) {
    unsigned char xor = 0;
    int fd;
    char * pathname = "", buff[1] = "";
    ssize_t count = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s [filepath]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pathname = argv[1];
    double t = Time_GetSeconds();
    if ((fd = open(pathname, O_RDONLY)) == -1)
        handle_error("open");

    while ((count = read(fd, buff, 1)) != -1 && count != 0) {
        if (buff[0] != '\n')
            xor ^= (buff[0] - '0');
    }

    printf("XOR-based checksum: %d\n", xor);
    printf("time(seconds): %f\n", Time_GetSeconds() - t);
    close(fd);

    return 0;
}
