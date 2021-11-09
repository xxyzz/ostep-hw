CFLAGS := -Wall -pthread

all: fork-join.out rendezvous.out barrier.out reader-writer.out \
	reader-writer-nostarve.out mutex-nostarve.out cigarette_smokers.out \
	dining_savages.out barbershop.out FIFO_barbershop.out Hilzers_barbershop.out

%.out: %.c common_threads.h
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -f ./*.out
