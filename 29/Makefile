CC := clang
OSFLAG := -D $(shell uname -s)
CFLAGS := -Wall -Wextra -pthread

all: approximate_counter.out queue.out simple_counter.out \
	standard_list.out hand_over_hand.out btree.out

approximate_counter.out: approximate_counter.c
	$(CC) -o $@ $^ $(CFLAGS) -lm $(OSFLAG)

queue.out: concurrent_queue.c
	$(CC) -o $@ $^ $(CFLAGS)

simple_counter.out: simple_concurrent_counter.c thread_helper.h
	$(CC) -o $@ $< $(CFLAGS) $(OSFLAG)

standard_list.out: concurrent-linked-list.c
	$(CC) -o $@ $^ $(CFLAGS)

hand_over_hand.out: hand-over-hand-locking-list.c
	$(CC) -o $@ $^ $(CFLAGS)

btree.out: btree.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f ./*.out
