#include <arpa/inet.h>  // htons, htonl
#include <fcntl.h>      // open
#include <netinet/in.h> // sockaddr_in
#include <stdio.h>      // perror
#include <stdlib.h>     // exit, atoi
#include <string.h>     // memset, memcpy, strerror
#include <sys/socket.h> // socket, bind, listen, AF_INET

#define LISTEN_BACKLOG 80 // maxium length of the pending connections queue
#define SOCKET_PORT 8080
#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int init_socket(int is_server, int nonblock) {
  struct sockaddr_in addr;
  int socket_flags = SOCK_STREAM;
#ifdef __linux__
  if (nonblock)
    socket_flags |= SOCK_NONBLOCK;
#endif
  int sfd = socket(AF_INET, socket_flags, 0);
  if (sfd == -1)
    handle_error("socket");
#ifndef __linux__
  if (nonblock) {
    if (fcntl(sfd, F_SETFL, O_NONBLOCK))
      handle_error("fcntl");
  }
#endif
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SOCKET_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (is_server) {
    const int optval = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
      handle_error("setsocketopt");
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
      handle_error("bind");
    if (listen(sfd, LISTEN_BACKLOG) == -1)
      handle_error("listen");
  } else if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    handle_error("connect");
  return sfd;
}
