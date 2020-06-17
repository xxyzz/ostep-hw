CC := clang10
SHELL := /usr/local/bin/zsh
OSFLAG :=
kernel_name := $(shell uname -s)

ifeq ($(kernel_name), Linux)
	OSFLAG = -D Linux
else
	ifeq ($(kernel_name), FreeBSD)
		OSFLAG = -D FreeBSD
	endif
endif

all: counter queue simple_counter standard_list hand_over_hand btree

counter: approximate_counter.c
	gcc -o sloppy_counter.out approximate_counter.c -Wall -pthread

queue: concurrent_queue.c
	gcc -o queue.out concurrent_queue.c -Wall -pthread

simple_counter: simple_concurrent_counter.c thread_helper.h
	$(CC) -o simple_counter.out simple_concurrent_counter.c -Weverything -pthread $(OSFLAG)

standard_list: concurrent-linked-list.c
	gcc -o standard_list.out concurrent-linked-list.c -Wall -pthread

hand_over_hand: hand-over-hand-locking-list.c
	gcc -o hand_over_hand.out hand-over-hand-locking-list.c -Wall -pthread

btree: btree.c
	gcc -o btree.out btree.c -Wall -pthread
