#include "connection.h"
#include <errno.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <time.h>   // clock_gettime
#include <unistd.h> // close

// man epoll
// The Linux programming interface, chapter 63.4.3
int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s numReqs", argv[0]);
    exit(EXIT_FAILURE);
  }
  struct timespec start, end;
  if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    handle_error("clock_gettime");

  int numReqs = atoi(argv[1]);
  int sfd = init_socket(1, 0);
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
          char buff[BUFSIZ] = "";
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
  if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    handle_error("clock_gettime");
  // nanoseconds
  printf("%f\n",
         ((end.tv_sec - start.tv_sec) * 1E9 + end.tv_nsec - start.tv_nsec));
  return 0;
}
