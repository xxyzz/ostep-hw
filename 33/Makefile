CC = clang
CFLAGS = -Wall -Wextra
UNAME := $(shell uname -s)
ifeq ($(UNAME), Linux)
	CFLAGS += -lrt  # Linux needs it for aio, BSD doesn't
endif

all: TCPServer.out TCPClient.out TCPServer-sync.out

%.out: %.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f ./*.out
