all: counter queue

counter: approximate_counter.c
	gcc -o counter.out approximate_counter.c -Wall -pthread

queue: concurrent_queue.c
	gcc -o queue.out concurrent_queue.c -Wall -pthread
