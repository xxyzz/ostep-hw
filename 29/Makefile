CC := clang
SHELL := $(shell which zsh)
OSFLAG := -D $(shell uname -s)
CFLAGS = -Weverything -pthread

all: approximate_counter queue simple_counter standard_list hand_over_hand btree

approximate_counter: approximate_counter.c
	$(CC) -o approximate_counter.out approximate_counter.c $(CFLAGS) -lm $(OSFLAG)

queue: concurrent_queue.c
	$(CC) -o queue.out concurrent_queue.c $(CFLAGS)

simple_counter: simple_concurrent_counter.c thread_helper.h
	$(CC) -o simple_counter.out simple_concurrent_counter.c $(CFLAGS) $(OSFLAG)

standard_list: concurrent-linked-list.c
	$(CC) -o standard_list.out concurrent-linked-list.c $(CFLAGS)

hand_over_hand: hand-over-hand-locking-list.c
	$(CC) -o hand_over_hand.out hand-over-hand-locking-list.c $(CFLAGS)

btree: btree.c
	$(CC) -o btree.out btree.c $(CFLAGS)
