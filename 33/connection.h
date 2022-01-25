#include <stdlib.h> // perror, exit, atoi

#define LISTEN_BACKLOG 80 // maxium length of the pending connections queue
#define SOCKET_PORT 8080
#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)
