CC = clang
CFLAGS = -Wall -Wextra -pthread

all: UDP-client.out UDP-server.out

%.out: %.c UDP-lib.h
	$(CC) -o $@ $< $(CFLAGS)
