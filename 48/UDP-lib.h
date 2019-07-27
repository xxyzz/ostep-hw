#define _POSIX_C_SOURCE 201112L    // getnameinfo >= 201112L, sem_timedwait >= 200112L, clock_gettime >= 199309L

#include <sys/types.h>     // some historical (BSD) implementations required it
#include <sys/socket.h>    // socket(), bind(), connect(), sendto(), recvfrom(), AF_UNSPEC, SOCK_DGRAM
#include <netdb.h>         // getaddrinfo(), getnameinfo(), AI_PASSIVE
#include <string.h>        // memset(), strlen()
#include <unistd.h>        // close()
#include <stdlib.h>        // exit(), EXIT_FAILURE
#include <stdio.h>         // printf(), fprintf(), perror
#include <string.h>        // strncpy()
#include <fcntl.h>         // For O_* constants
#include <sys/stat.h>      // For mode constants
#include <semaphore.h>
#include <time.h>          // timespec, clock_gettime
#include <errno.h>

#define BUFFER_SIZE 65507
#define TIMEOUT_SECONDS 10

char * client_sem_name = "/client_sem";
sem_t * client_sem;
struct timespec ts;

int
UDP_Open(char *hostName, char *port, int server)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family    = AF_UNSPEC;     /* Allow IPv4 or Ipv6 */
    /* Datagram socket (connectionless, unreliable 
       messages of a fixed maximum length) */
    hints.ai_socktype  = SOCK_DGRAM;
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

    if (!server && (client_sem = sem_open(client_sem_name, O_CREAT, S_IRWXU, 1)) == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    return sfd;
}

ssize_t
UDP_Write(int sfd, char *buffer, int nread, struct sockaddr *peer_addr, int peer_addr_len)
{
    if (nread > BUFFER_SIZE) {
        fprintf(stderr, "Exceed max buffer size\n");
        exit(EXIT_FAILURE);
    }
    
    int s = 0;
    
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    ts.tv_sec += TIMEOUT_SECONDS;
    // wait ack
    errno = 0;
    int isAsk = 0;
    if ((isAsk = strncmp(buffer, "ack", 4)) != 0)
        s = sem_timedwait(client_sem, &ts);

    if (s == -1) {
        if (errno == ETIMEDOUT) {
            // retry
            return UDP_Write(sfd, buffer, nread, peer_addr, peer_addr_len);
        } else {
            perror("sem_timedwait");
            exit(EXIT_FAILURE);
        }
    } else {
        if (isAsk != 0)
            sem_post(client_sem);
        return sendto(sfd, buffer, nread, 0, peer_addr, peer_addr_len);
    }
}

ssize_t
UDP_Read(int sfd, char *buffer, struct sockaddr *peer_addr, socklen_t *peer_addr_len)
{
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t recv_bytes = recvfrom(sfd, buffer, BUFFER_SIZE, 0, peer_addr, peer_addr_len);
    if (recv_bytes > 0 && strncmp(buffer, "ack", 4) != 0) {
        printf("Send ack\n");
        UDP_Write(sfd, "ack", 4, peer_addr, *peer_addr_len);    // send ask
    }
    return recv_bytes;
}

void
closeSem() {
    if (sem_close(client_sem) != 0) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(client_sem_name) != 0) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }
}
