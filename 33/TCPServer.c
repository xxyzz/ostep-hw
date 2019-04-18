#include <stdio.h>
#include <stdlib.h>            // perror(), exit(), calloc(), malloc()
#include <sys/types.h>         // see NOTES in man 2 socket
#include <sys/socket.h>        // socket(), bind(), listen(), accept(), send(), recv(), AF_INET, SOCK_STREAM
#include <sys/select.h>
#include <netinet/in.h>        // sockaddr_in, INADDR_ANY
#include <string.h>            // memset(), strlen(), strncmp()
#include <arpa/inet.h>         // htonl(), htons()
#include <time.h>              // time(), strftime(), localtime()
#include <unistd.h>            // read(), close()
#include <fcntl.h>             // open()
#include <aio.h>
#include <errno.h>             // EINPROGRESS, EINTR
#include <signal.h>

#define BUFFSIZE          1024
#define PORT              8080
#define LISTEN_BACKLOG    80   // Maximum length of the pending connections queue
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Signal used to notify I/O completion https://www.gnu.org/software/libc/manual/html_node/Miscellaneous-Signals.html
#define IO_SIGNAL SIGUSR1

struct ioRequest {    // Tracking I/O request
    int           cfd;
    int           status;
    struct aiocb *aiocbp;
};

fd_set afds, rfds;     // active set, read set
char buff[BUFFSIZE];

// static void aioSigHandler(int sig, siginfo_t *si, void *ucontext) {
//     if (si->si_code == SI_ASYNCIO) {
//         printf("Send file contents %s\n", buff);
//         struct ioRequest *ioReq = si->si_value.sival_ptr;
//         if (send(ioReq->cfd, (char *) ioReq->aiocbp->aio_buf, BUFFSIZE, 0) == -1)
//             handle_error("send");

//         close(ioReq->aiocbp->aio_fildes);
//         close(ioReq->cfd);
//         FD_CLR(ioReq->cfd, &afds);
//     }
// }

int main(int argc, char *argv[]) {
    int sfd, errno;
    int numReqs = 0;     // Total number of queued I/O requests
    int openReqs = 0;    // Number of I/O requests still in progress
    struct sockaddr_in my_addr, peer_addr;    // man 7 ip
    socklen_t peer_addr_size;
    struct ioRequest *ioList;
    struct aiocb     *aiocbList;
    // struct sigaction  sa;

    // Allocate arrays
    if ((ioList = calloc(LISTEN_BACKLOG, sizeof(struct ioRequest))) == NULL)
        handle_error("calloc");

    if ((aiocbList = calloc(LISTEN_BACKLOG, sizeof(struct aiocb))) == NULL)
        handle_error("calloc");

    // Establish I/O completion signal
    // sa.sa_flags = SA_RESTART | SA_SIGINFO;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_sigaction = aioSigHandler;
    // if (sigaction(IO_SIGNAL, &sa, NULL) == -1)
    //     handle_error("sigaction");

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

    FD_ZERO(&afds);        // clear set
    FD_SET(sfd, &afds);    // add file descriptor
    while (1) {
        rfds = afds;
        errno = 0;
        if (select(FD_SETSIZE, &rfds, NULL, NULL, NULL) == -1) {
            handle_error("select");
            // if (errno == EINTR)
            //     continue;
            // else
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &rfds)) {
                if (i == sfd) {
                    int cfd;
                    cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
                    if (cfd == -1)
                        handle_error("accept");
                    FD_SET(cfd, &afds);
                    openReqs++;
                } else {
                    memset(buff, 0, BUFFSIZE);
                    if (recv(i, buff, BUFFSIZE, 0) == -1)
                        handle_error("recv");

                    ioList[numReqs].cfd = i;
                    ioList[numReqs].aiocbp = &aiocbList[numReqs];
                    ioList[numReqs].status = EINPROGRESS;

                    if ((ioList[numReqs].aiocbp->aio_fildes = open(buff, O_RDONLY)) == -1)
                        handle_error("open");

                    if ((ioList[numReqs].aiocbp->aio_buf = malloc(BUFFSIZE)) == NULL)
                        handle_error("malloc");

                    ioList[numReqs].aiocbp->aio_nbytes = BUFFSIZE;
                    ioList[numReqs].aiocbp->aio_offset = 0;
                    ioList[numReqs].aiocbp->aio_reqprio = 0;
                    // ioList[numReqs].aiocbp->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
                    // ioList[numReqs].aiocbp->aio_sigevent.sigev_signo = IO_SIGNAL;
                    // ioList[numReqs].aiocbp->aio_sigevent.sigev_value.sival_ptr = &ioList[numReqs];

                    if (aio_read(ioList[numReqs].aiocbp) == -1)
                        handle_error("aio_read");

                    numReqs++;
                }
            }
        }

        if (openReqs > 0) {
            for(size_t i = 0; i < numReqs; i++) {
                if (ioList[i].status == EINPROGRESS) {
                    ioList[i].status = aio_error(ioList[i].aiocbp);
                    if (ioList[i].status == 0) {
                        if (send(ioList[i].cfd, (char *) ioList[i].aiocbp->aio_buf, BUFFSIZE, 0) == -1)
                            handle_error("send");
                        printf("Send file contents %s\n", (char *) ioList[i].aiocbp->aio_buf);
                        close(ioList[i].aiocbp->aio_fildes);
                        close(ioList[i].cfd);
                        FD_CLR(ioList[i].cfd, &afds);
                    }
                    if (ioList[i].status != EINPROGRESS)
                        openReqs--;
                }
            }
        }
    }
    return 0;
}
