CC = clang
CFLAGS = -Wall -Wextra

all: null.out forget_free.out size_100.out free_then_print.out vector.out

clean:
	rm -f ./*.out

vector.out: vector.c vector.h

%.out: %.c
	$(CC) -o $@ $< $(CFLAGS)
