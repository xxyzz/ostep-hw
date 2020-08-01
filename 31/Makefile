CC := clang
CFLAGS := -Weverything -pthread

all: fork-join.out rendezvous.out barrier.out reader-writer.out \
	reader-writer-nostarve.out mutex-nostarve.out

%.out: %.c common_threads.h
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f ./*.out
