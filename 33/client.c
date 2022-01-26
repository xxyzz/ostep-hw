#include "connection.h"
#include <unistd.h> // close

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s file_path\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int sfd = init_socket(0);
  if (send(sfd, argv[1], strlen(argv[1]), 0) == -1)
    handle_error("send");
  char buf[BUFSIZ] = "";
  if (recv(sfd, buf, BUFSIZ, 0) == -1)
    handle_error("recv");
  printf("%s\n", buf);
  close(sfd);
  return 0;
}
