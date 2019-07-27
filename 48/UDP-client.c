#include "UDP-lib.h"

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd = UDP_Open(argv[1], "10000", 0);
    ssize_t nread;
    char buf[BUFFER_SIZE] = "first hello";

    for(size_t i = 0; i < 2; i++) {
        printf("Send: %s\n", buf);
        if (UDP_Write(sfd, buf, strlen(buf), NULL, 0) != strlen(buf)) {
            fprintf(stderr, "partial/failed write\n");
            exit(EXIT_FAILURE);
        }

        nread = UDP_Read(sfd, buf, NULL, 0);
        if (nread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Received %zd bytes: %s\n", nread, buf);
        strncpy(buf, "hello", 6);
    }

    close(sfd);
    closeSem();
    exit(EXIT_SUCCESS);
}
