CFLAGS = -Wall -Wextra
LDFLAGS = -luring

all: client.out server_epoll.out server_io_uring.out

%.out: %.c
	$(CC) -o $@ $^ $(CFLAGS)

server_io_uring.out: server_io_uring.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f ./*.out
