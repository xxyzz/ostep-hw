#include "connection.h"
#include <pthread.h>
#include <unistd.h> // close

void *send_requests(void *arg) {
  char *file_name = (char *)arg;
  int sfd = init_socket(0, 0);
  if (send(sfd, file_name, strlen(file_name), 0) == -1)
    handle_error("send");
  char buf[BUFSIZ] = "";
  if (recv(sfd, buf, BUFSIZ, 0) == -1)
    handle_error("recv");
  printf("%s\n", buf);
  close(sfd);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s file_path num_threads\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int num_threads = atoi(argv[2]);
  if (num_threads > 1) {
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++) {
      if (pthread_create(&threads[i], NULL, send_requests, argv[1]))
        handle_error("pthread_create");
    }
    for (int i = 0; i < num_threads; i++) {
      if (pthread_join(threads[i], NULL))
        handle_error("pthread_join");
    } 
  } else
    send_requests(argv[1]);
  return 0;
}
