CC = clang
CFLAGS = -Weverything

all: stat.out myls.out mytail.out myfind.out

%.out: %.c
	$(CC) -o $@ $^ $(CFLAGS)
