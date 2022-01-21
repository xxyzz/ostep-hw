#include <stdlib.h> // perror, exit, atoi

#define SOCKET_NAME "/tmp/rtfm.socket"
#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)
