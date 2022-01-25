#include "connection.h"
#include <arpa/inet.h>  // htons, htonl
#include <netinet/in.h> // sockaddr_in
#include <stdio.h>
#include <string.h>     // memset
#include <sys/socket.h> // socket, connect
#include <unistd.h>     // close

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s file_path\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in server_addr;
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SOCKET_PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
    handle_error("connect");
  if (send(sfd, argv[1], strlen(argv[1]), 0) == -1)
    handle_error("send");
  char buf[BUFSIZ] = "";
  if (recv(sfd, buf, BUFSIZ, 0) == -1)
    handle_error("recv");
  printf("%s\n", buf);
  close(sfd);
  return 0;
}
