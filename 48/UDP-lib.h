#include <sys/types.h>
#include <sys/socket.h>    // socket(), bind(), connect(), sendto(), recvfrom(), AF_UNSPEC, SOCK_DGRAM
#include <netdb.h>         // getaddrinfo(), getnameinfo(), AI_PASSIVE
#include <string.h>        // memset(), strlen()
#include <unistd.h>        // close()
#include <stdlib.h>        // exit(), EXIT_FAILURE
#include <stdio.h>         // sprintf()

#define BUFFER_SIZE 500

int UDP_Open(char *hostName, char *port, int server) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family    = AF_UNSPEC;     /* Allow IPv4 or Ipv6 */
    hints.ai_socktype  = SOCK_DGRAM;    /* Datagram socket */
    hints.ai_flags     = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol  = 0;             /* Any protocal */
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;

    s = getaddrinfo(hostName, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2) for 
       server or connect(2) for client. If socket(2) (or 
       bind(2) or connect(2)) fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (server && bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */
        else if (!server && connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        char *error_message = server ? "Could not bind" : "Could not connect";
        fprintf(stderr, "%s\n", error_message);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    return sfd;
}

int UDP_Write(int sfd, char *buffer, int nread, struct sockaddr *peer_addr, int peer_addr_len) {
    return sendto(sfd, buffer, nread, 0, peer_addr, peer_addr_len);
}

int UDP_Read(int sfd, char *buffer, struct sockaddr *peer_addr, socklen_t *peer_addr_len) {
    return recvfrom(sfd, buffer, BUFFER_SIZE, 0, peer_addr, peer_addr_len);
}
