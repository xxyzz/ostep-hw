CFLAGS = -Wall -Wextra

ALL_TARGETS = client.out server_libevent.out

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ALL_TARGETS += server_io_uring.out server_epoll.out
endif

all: $(ALL_TARGETS)

%.out: %.c connection.h
	$(CC) -o $@ $< $(CFLAGS)

server_io_uring.out: server_io_uring.c connection.h
	$(CC) -o $@ $< $(CFLAGS) -luring

client.out: client.c connection.h
	$(CC) -o $@ $< $(CFLAGS) -pthread

server_libevent.out: server_libevent.c connection.h
	$(CC) -o $@ $< $(CFLAGS) -levent_core

clean:
	rm -f ./*.out
