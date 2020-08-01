CC = clang
CFLAGS = -Weverything -pthread

all: UDP-client.out UDP-server.out

%.out: %.c UDP-lib.h
	$(CC) -o $@ $< $(CFLAGS)
