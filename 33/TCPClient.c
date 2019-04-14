#include <stdio.h>
#include <stdlib.h>         // perror(), exit()
#include <sys/types.h>      // for BSD, man 2 socket, see NOTES
#include <sys/socket.h>     // socket(), connect(), send(), recv(), AF_INET, SOCK_STREAM
#include <netinet/in.h>     // sockaddr_in, INADDR_ANY
#include <string.h>         // memset()
#include <arpa/inet.h>      // htonl(), htons(), inet_addr()
#include <unistd.h>         // close()

#define BUFFSIZE 1024
#define PORT     8080
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    int sfd;
    struct sockaddr_in server_addr;                         // man 7 ip
    sfd = socket(AF_INET, SOCK_STREAM, 0);                  // tcp socket
    if (sfd == -1)
        handle_error("socket");

    memset(&server_addr, 0, sizeof(struct sockaddr_in));    // clear structure
    server_addr.sin_family      = AF_INET;                  // address family, IPV4
    server_addr.sin_port        = htons(PORT);              // port in network byte order
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");   // internet address

    if (connect(sfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) != 0)
        handle_error("connect");

    char buff[BUFFSIZE];
    memset(buff, 0, sizeof(buff));
    printf("Request time\n");
    if (send(sfd, "time", strlen("time"), 0) == -1)
        handle_error("send");
    memset(buff, 0, sizeof(buff));
    if (recv(sfd, buff, BUFFSIZE, 0) == -1)
        handle_error("recv");
    printf("From Server: %s\n", buff);
    memset(buff, 0, sizeof(buff));
    printf("Exit\n");
    if (send(sfd, "exit", strlen("exit"), 0) == -1)
        handle_error("send");

    close(sfd);

    return 0;
}
