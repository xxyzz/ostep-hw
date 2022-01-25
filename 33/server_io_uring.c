#include "connection.h"
#include <fcntl.h> // open
#include <liburing.h>
#include <netinet/in.h> // sockaddr_in
#include <stdio.h>
#include <stdlib.h>     // calloc, free
#include <string.h>     // memset, memcpy
#include <sys/socket.h> // socket, bind, listen, accept, recv, AF_INET
#include <sys/stat.h>
#include <unistd.h> // close, pipe

// https://kernel.dk/io_uring.pdf
// https://github.com/axboe/liburing
// https://github.com/torvalds/linux/blob/master/fs/io_uring.c
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/io_uring.h

struct user_data {
  char buf[BUFSIZ];
  int pipefd[2];
  off_t size;
  int io_op;
  int socket_fd;
  int file_fd;
};

void prep_accept(struct io_uring *ring, struct io_uring_sqe *sqe, int sfd) {
  struct user_data *data = malloc(sizeof(struct user_data));
  if (data == NULL)
    handle_error("calloc");
  data->io_op = IORING_OP_ACCEPT;
  io_uring_prep_accept(sqe, sfd, NULL, NULL, 0);
  // https://github.com/axboe/liburing/commit/8ecd3fd959634df81d66af8b3a69c16202a014e8
  io_uring_sqe_set_data(sqe, data);
  if (io_uring_submit(ring) < 0) {
    free(data);
    handle_error("io_muring_submit");
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s numReqs", argv[0]);
    exit(EXIT_FAILURE);
  }
  int numReqs = atoi(argv[1]);

  struct sockaddr_in my_addr;
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");
  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(SOCKET_PORT);
  my_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  const int optval = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval,
                 sizeof(optval)))
    handle_error("setsocketopt");
  if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
    handle_error("bind");
  if (listen(sfd, LISTEN_BACKLOG) == -1)
    handle_error("listen");

  struct io_uring ring;
  if (io_uring_queue_init(LISTEN_BACKLOG, &ring, 0))
    handle_error("io_uring_queue_init");
  struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  if (sqe == NULL)
    handle_error("io_uring_get_sqe");
  prep_accept(&ring, sqe, sfd);

  while (numReqs > 0) {
    struct io_uring_cqe *cqe;
    if (io_uring_wait_cqe(&ring, &cqe))
      handle_error("io_uring_wait_cqe");
    struct user_data *data = io_uring_cqe_get_data(cqe);
    if (data == NULL) {
      fprintf(stderr, "cqe->user_data is NULL\n");
      exit(EXIT_FAILURE);
    }
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (sqe == NULL)
      handle_error("io_uring_get_sqe");

    switch (data->io_op) {
    case IORING_OP_ACCEPT:
      if (cqe->res == -1)
        handle_error("accept");
      struct user_data *new_data = calloc(1, sizeof(struct user_data));
      if (new_data == NULL)
        handle_error("calloc");
      new_data->io_op = IORING_OP_RECV;
      new_data->socket_fd = cqe->res;
      io_uring_prep_recv(sqe, cqe->res, new_data->buf, BUFSIZ, 0);
      io_uring_sqe_set_data(sqe, new_data);
      if (io_uring_submit(&ring) < 0) {
        free(new_data);
        handle_error("io_uring_submit");
      }
      break;
    case IORING_OP_RECV:
      if (cqe->res == -1)
        handle_error("recv");

      int file_fd = open(data->buf, O_RDONLY);
      if (file_fd == -1)
        handle_error("open");
      struct stat statbuf;
      if (fstat(file_fd, &statbuf) == -1)
        handle_error("fstat");
      int pipefd[2];
      if (pipe(pipefd) == -1)
        handle_error("pipe");

      struct user_data *new_user_data = malloc(sizeof(struct user_data));
      if (new_user_data == NULL)
        handle_error("malloc");
      new_user_data->io_op = IORING_OP_SPLICE;
      new_user_data->socket_fd = data->socket_fd;
      new_user_data->file_fd = file_fd;
      new_user_data->size = statbuf.st_size;
      memcpy(new_user_data->pipefd, pipefd, sizeof(pipefd));
      // https://github.com/axboe/liburing/blob/29ff69397fa13478b5619201347c51159874279e/src/include/liburing.h#L289-L307
      io_uring_prep_splice(sqe, file_fd, -1, pipefd[1], -1, statbuf.st_size, 0);
      io_uring_sqe_set_data(sqe, new_user_data);
      if (io_uring_submit(&ring) < 0) {
        free(new_user_data);
        handle_error("io_uring_submit");
      }
      break;
    case IORING_OP_SPLICE:
      if (cqe->res == -1)
        handle_error("splice");

      if (data->size != -1) {
        close(data->file_fd);
        close(data->pipefd[1]);
        struct user_data *new_data = malloc(sizeof(struct user_data));
        if (new_data == NULL)
          handle_error("malloc");
        memcpy(new_data, data, sizeof(struct user_data));
        new_data->size = -1;
        io_uring_prep_splice(sqe, data->pipefd[0], -1, data->socket_fd, -1,
                             data->size, 0);
        io_uring_sqe_set_data(sqe, new_data);
        if (io_uring_submit(&ring) < 0) {
          free(new_data);
          handle_error("io_uring_submit");
        }
      } else {
        close(data->pipefd[0]);
        close(data->socket_fd);
        if (--numReqs > 0)
          prep_accept(&ring, sqe, sfd);
      }
      break;
    default:
      handle_error("Unknown I/O");
    }
    io_uring_cqe_seen(&ring, cqe);
    free(data);
  }
  io_uring_queue_exit(&ring);
  close(sfd);
  return 0;
}
