CC := clang
CFLAGS := -Wall -Wextra -pthread

all: fork-join.out rendezvous.out barrier.out reader-writer.out \
	reader-writer-nostarve.out mutex-nostarve.out

%.out: %.c common_threads.h
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f ./*.out
