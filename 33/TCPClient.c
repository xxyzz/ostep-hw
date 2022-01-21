#include "connection.h"
#include <fcntl.h> // open
#include <stdio.h>
#include <string.h> // memset
#include <string.h> // strncpy
#include <sys/sendfile.h>
#include <sys/socket.h> // socket, connect
#include <sys/stat.h>   // fstat
#include <sys/un.h>     // sockaddr_un
#include <unistd.h>     // read, close

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s file_path\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  struct sockaddr_un server_addr;
  int sfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (sfd == -1)
    handle_error("socket");
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_NAME, sizeof(server_addr.sun_path) - 1);
  if (connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
    handle_error("connect");
  if (send(sfd, argv[1], strlen(argv[1]), 0) == -1)
    handle_error("send");
  char buf[BUFSIZ];
  if (recv(sfd, buf, BUFSIZ, 0) == -1)
    handle_error("recv");
  printf("%s\n", buf);
  close(sfd);
  return 0;
}
