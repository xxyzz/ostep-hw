all: counter queue simple_counter

counter: approximate_counter.c
	gcc -o counter.out approximate_counter.c -Wall -pthread

queue: concurrent_queue.c
	gcc -o queue.out concurrent_queue.c -Wall -pthread

simple_counter: simple_concurrent_counter.c
	gcc -o simple_counter.out simple_concurrent_counter.c -Wall -pthread
