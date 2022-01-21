CFLAGS = -Wall -Wextra

all: TCPClient.out TCPServer_sync.out

%.out: %.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f ./*.out
