CC = clang
CFLAGS = -Wall -Wextra

all: stat.out myls.out mytail.out myfind.out

%.out: %.c
	$(CC) -o $@ $^ $(CFLAGS)
