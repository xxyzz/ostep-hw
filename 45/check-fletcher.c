#include <stdio.h>     // fprintf, perror
#include <stdlib.h>    // exit
#include <fcntl.h>     // open
#include <unistd.h>    // read
#include "common.h"

int main(int argc, char *argv[]) {
    int fd, fletcher_a = 0, fletcher_b = 0;
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
        if (buff[0] != '\n') {
            fletcher_a = (buff[0] - '0' + fletcher_a) % 255;
            fletcher_b = (fletcher_a + fletcher_b) % 255;
        }
    }

    printf("Fletcher checksum: %d, %d\n", fletcher_a, fletcher_b);
    printf("time(seconds): %f\n", Time_GetSeconds() - t);
    close(fd);

    return 0;
}
