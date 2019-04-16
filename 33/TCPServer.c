#include <stdio.h>
#include <stdlib.h>            // perror(), exit()
#include <sys/types.h>         // see NOTES in man 2 socket
#include <sys/socket.h>        // socket(), bind(), listen(), accept(), send(), recv(), AF_INET, SOCK_STREAM
#include <sys/select.h>
#include <netinet/in.h>        // sockaddr_in, INADDR_ANY
#include <string.h>            // memset(), strlen(), strncmp()
#include <arpa/inet.h>         // htonl(), htons()
#include <time.h>              // time(), strftime(), localtime()
#include <unistd.h>            // open(), read(), close()

#define BUFFSIZE          1024
#define PORT              8080
#define LISTEN_BACKLOG    80   // maxium length of the pending connections queue
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int sfd;
    struct sockaddr_in my_addr, peer_addr;              // man 7 ip
    socklen_t peer_addr_size;
    sfd = socket(AF_INET, SOCK_STREAM, 0);              // tcp socket
    if (sfd == -1)
        handle_error("socket");

    memset(&my_addr, 0, sizeof(struct sockaddr_in));    // clear structure
    my_addr.sin_family      = AF_INET;                  // address family, IPV4
    my_addr.sin_port        = htons(PORT);              // port in network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY;               // internet address, bind socket to all local interfaces

    if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1)
        handle_error("bind");

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        handle_error("listen");

    peer_addr_size = sizeof(struct sockaddr_in);

    char buff[BUFFSIZE];
    fd_set afds, rfds;     // active set, read set
    FD_ZERO(&afds);        // clear set
    FD_SET(sfd, &afds);    // add file descriptor
    while (1) {
        rfds = afds;
        if (select(FD_SETSIZE, &rfds, NULL, NULL, NULL) == -1)
            handle_error("select");

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &rfds)) {
                if (i == sfd) {
                    int cfd;
                    cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
                    if (cfd == -1)
                        handle_error("accept");
                    FD_SET(cfd, &afds);
                } else {
                    memset(buff, 0, BUFFSIZE);
                    if (recv(i, buff, BUFFSIZE, 0) == -1)
                        handle_error("recv");

                    if (strncmp("time", buff, strlen("time")) == 0) {
                        memset(buff, 0, BUFFSIZE);
                        time_t now = time(NULL);
                        strftime(buff, BUFFSIZE, "%Y-%m-%d %H:%M:%S", localtime(&now));
                        printf("Send time %s\n", buff);

                        if (send(i, buff, BUFFSIZE, 0) == -1)
                            handle_error("send");
                    }

                    if (strncmp("exit", buff, strlen("exit")) == 0) {
                        printf("Server exit\n");
                        close(i);
                        FD_CLR(i, &afds);
                    }
                }
            }
        }
    }
    return 0;
}
