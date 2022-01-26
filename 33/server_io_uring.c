#include "connection.h"
#include <fcntl.h> // open
#include <liburing.h>
#include <stdlib.h> // calloc, free
#include <sys/stat.h>
#include <unistd.h> // close, pipe

// https://kernel.dk/io_uring.pdf
// https://github.com/axboe/liburing
// https://github.com/torvalds/linux/blob/master/fs/io_uring.c
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/io_uring.h
// https://lwn.net/Kernel/Index/#io_uring

struct user_data {
  char buf[BUFSIZ];
  int pipefd[2];
  off_t size;
  int io_op;
  int socket_fd;
  int file_fd;
};

int numAccepts = 0, numReqs = 0;

void prep_accept(struct io_uring *ring, int sfd) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  if (sqe == NULL)
    handle_error("io_uring_get_sqe");
  struct user_data *data = malloc(sizeof(struct user_data));
  if (data == NULL)
    handle_error("malloc");
  data->io_op = IORING_OP_ACCEPT;
  io_uring_prep_accept(sqe, sfd, NULL, NULL, 0);
  // https://github.com/axboe/liburing/commit/8ecd3fd959634df81d66af8b3a69c16202a014e8
  io_uring_sqe_set_data(sqe, data);
  if (io_uring_submit(ring) < 0) {
    free(data);
    handle_error("io_uring_submit");
  }
  numAccepts--;
}

void prep_recv(struct io_uring *ring, int sfd, int cfd) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  if (sqe == NULL)
    handle_error("io_uring_get_sqe");
  struct user_data *data = calloc(1, sizeof(struct user_data));
  if (data == NULL)
    handle_error("calloc");
  data->io_op = IORING_OP_RECV;
  data->socket_fd = cfd;
  io_uring_prep_recv(sqe, cfd, data->buf, BUFSIZ, 0);
  io_uring_sqe_set_data(sqe, data);
  if (numAccepts > 0)
    prep_accept(ring, sfd);
  else if (io_uring_submit(ring) < 0) {
    free(data);
    handle_error("io_uring_submit");
  }
}

void prep_first_splice(struct io_uring *ring, struct user_data *data) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  if (sqe == NULL)
    handle_error("io_uring_get_sqe");

  int file_fd = open(data->buf, O_RDONLY);
  if (file_fd == -1)
    handle_error("open");
  struct stat statbuf;
  if (fstat(file_fd, &statbuf) == -1)
    handle_error("fstat");
  int pipefd[2];
  if (pipe(pipefd) == -1)
    handle_error("pipe");

  struct user_data *new_data = malloc(sizeof(struct user_data));
  if (new_data == NULL)
    handle_error("malloc");
  new_data->io_op = IORING_OP_SPLICE;
  new_data->socket_fd = data->socket_fd;
  new_data->file_fd = file_fd;
  new_data->size = statbuf.st_size;
  memcpy(new_data->pipefd, pipefd, sizeof(pipefd));
  // https://github.com/axboe/liburing/blob/29ff69397fa13478b5619201347c51159874279e/src/include/liburing.h#L289-L307
  io_uring_prep_splice(sqe, file_fd, -1, pipefd[1], -1, statbuf.st_size, 0);
  io_uring_sqe_set_data(sqe, new_data);
  if (io_uring_submit(ring) < 0) {
    free(new_data);
    handle_error("io_uring_submit");
  }
}

void prep_second_splice(struct io_uring *ring, struct user_data *data) {
  if (data->size != -1) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    if (sqe == NULL)
      handle_error("io_uring_get_sqe");

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
    if (io_uring_submit(ring) < 0) {
      free(new_data);
      handle_error("io_uring_submit");
    }
  } else {
    close(data->pipefd[0]);
    close(data->socket_fd);
    numReqs--;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s numReqs\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  numReqs = atoi(argv[1]);
  if (numReqs <= 0) {
    fprintf(stderr, "Get out\n");
    exit(EXIT_FAILURE);
  }
  numAccepts = numReqs;
  int sfd = init_socket(1);
  struct io_uring ring;
  if (io_uring_queue_init(LISTEN_BACKLOG, &ring, 0))
    handle_error("io_uring_queue_init");
  prep_accept(&ring, sfd);

  while (numReqs > 0) {
    struct io_uring_cqe *cqe;
    if (io_uring_wait_cqe(&ring, &cqe))
      handle_error("io_uring_wait_cqe");
    if (cqe->res < 0) {
      fprintf(stderr, "I/O error: %s\n", strerror(-cqe->res));
      exit(EXIT_FAILURE);
    }
    struct user_data *data = io_uring_cqe_get_data(cqe);
    if (data == NULL) {
      fprintf(stderr, "cqe->user_data is NULL\n");
      exit(EXIT_FAILURE);
    }

    switch (data->io_op) {
    case IORING_OP_ACCEPT:
      prep_recv(&ring, sfd, cqe->res);
      break;
    case IORING_OP_RECV:
      prep_first_splice(&ring, data);
      break;
    case IORING_OP_SPLICE:
      prep_second_splice(&ring, data);
      break;
    default:
      handle_error("Unknown I/O");
    }
    free(data);
    io_uring_cqe_seen(&ring, cqe);
  }
  io_uring_queue_exit(&ring);
  close(sfd);
  return 0;
}
