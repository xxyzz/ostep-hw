#ifdef __linux__
#define _GNU_SOURCE // accept4
#endif
#include "connection.h"
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <time.h>   // clock_gettime
#include <unistd.h> // close

// https://github.com/libevent/libevent
int num_reqs = 0;

void readcb(struct bufferevent *bev, void *ctx) {
  char buf[BUFSIZ] = "";
  bufferevent_read(bev, buf, BUFSIZ);
  int fd = open(buf, O_RDONLY | O_NONBLOCK);
  if (fd == -1)
    handle_error("open");
  struct evbuffer *output = bufferevent_get_output(bev);
  evbuffer_set_flags(output, EVBUFFER_FLAG_DRAINS_TO_FD);
  evbuffer_add_file(output, fd, 0, -1);
}

void errorcb(struct bufferevent *bev, short error, void *ctx) {
  if (error & BEV_EVENT_EOF) {
    if (--num_reqs == 0) {
      struct event_base *base = bufferevent_get_base(bev);
      event_base_loopexit(base, NULL);
    }
  } else if (error & BEV_EVENT_ERROR) {
    fprintf(stderr, "error: %s\n", strerror(error));
  }
  bufferevent_free(bev);
}

void do_accept(int sfd, short event, void *arg) {
  struct event_base *base = arg;
#ifdef __linux__
  int cfd = accept4(sfd, NULL, NULL, SOCK_NONBLOCK);
  if (cfd == -1)
    handle_error("accept4");
#else
  int cfd = accept(sfd, NULL, NULL);
  if (cfd == -1)
    handle_error("accept");
  evutil_make_socket_nonblocking(cfd);
#endif
  struct bufferevent *bev =
      bufferevent_socket_new(base, cfd, BEV_OPT_CLOSE_ON_FREE);
  bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
  bufferevent_enable(bev, EV_READ | EV_WRITE);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s numReqs", argv[0]);
    exit(EXIT_FAILURE);
  }
  struct timespec start, end;
  if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    handle_error("clock_gettime");

  num_reqs = atoi(argv[1]);
  int sfd = init_socket(1, 1);

  struct event_base *base = event_base_new();
  struct event *listener_event =
      event_new(base, sfd, EV_READ | EV_PERSIST, do_accept, (void *)base);
  event_add(listener_event, NULL);
  event_base_dispatch(base);
  event_base_free(base);
  if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    handle_error("clock_gettime");
  // nanoseconds
  printf("%f\n",
         ((end.tv_sec - start.tv_sec) * 1E9 + end.tv_nsec - start.tv_nsec));
  return 0;
}
