all: counter cpus

counter: approximate_counter.c
	gcc -o a.out approximate_counter.c -Wall -pthread

cpus: cpus.c
	gcc -o b.out cpus.c -Wall
