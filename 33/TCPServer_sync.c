#include "connection.h"
#include <errno.h>
#include <fcntl.h>  // open
#include <stdio.h>  // remove
#include <string.h> // memset
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/socket.h> // socket, bind, listen, accept, recv, AF_UNIX
#include <sys/stat.h>
#include <sys/un.h> // sockaddr_un
#include <unistd.h> // close

#define LISTEN_BACKLOG 80 // maxium length of the pending connections queue

// man epoll
// The Linux programming interface, chapter 63.4.3
int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s numReqs", argv[0]);
    exit(EXIT_FAILURE);
  }
  int numReqs = atoi(argv[1]);

  struct sockaddr_un my_addr;
  int sfd = socket(AF_UNIX, SOCK_SEQPACKET, 0); // man 7 unix
  if (sfd == -1)
    handle_error("socket");
  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sun_family = AF_UNIX;
  strncpy(my_addr.sun_path, SOCKET_NAME, sizeof(my_addr.sun_path) - 1);
  if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
    if (errno == EADDRINUSE) {
      close(sfd);
      remove(SOCKET_NAME);
    }
    handle_error("bind");
  }
  if (listen(sfd, LISTEN_BACKLOG) == -1)
    handle_error("listen");

  int epfd = epoll_create1(0);
  if (epfd == -1)
    handle_error("epoll_create1");
  struct epoll_event ev;
  struct epoll_event evlist[LISTEN_BACKLOG];
  ev.events = EPOLLIN;
  ev.data.fd = sfd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev) == -1)
    handle_error("epoll_ctl");
  while (numReqs > 0) {
    int ready = epoll_wait(epfd, evlist, LISTEN_BACKLOG, -1);
    if (ready == -1) {
      if (errno == EINTR)
        continue; // Restart if interrupted by signal
      else
        handle_error("epoll_wait");
    }

    for (int i = 0; i < ready; i++) {
      if (evlist[i].events & EPOLLIN) {
        if (evlist[i].data.fd == sfd) {
          int cfd = accept(sfd, NULL, NULL);
          if (cfd == -1)
            handle_error("accept");
          ev.events = EPOLLIN;
          ev.data.fd = cfd;
          if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev) == -1)
            handle_error("epoll_ctl");
        } else {
          char buff[BUFSIZ];
          if (recv(evlist[i].data.fd, buff, BUFSIZ, 0) == -1)
            handle_error("recv");
          int fd = open(buff, O_RDONLY);
          if (fd == -1)
            handle_error("open");
          struct stat statbuf;
          if (fstat(fd, &statbuf) == -1)
            handle_error("fstat");
          if (sendfile(evlist[i].data.fd, fd, NULL, statbuf.st_size) == -1)
            handle_error("sendfile");
          close(fd);
          close(evlist[i].data.fd);
          numReqs--;
        }
      }
    }
  }
  close(sfd);
  remove(SOCKET_NAME);
  return 0;
}
