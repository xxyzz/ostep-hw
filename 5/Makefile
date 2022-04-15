CC ?= clang
CFLAGS = -Wall -Wextra
TARGETS := $(foreach i,$(shell seq 10),$i.out)

all: $(TARGETS)

%.out: %.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f ./*.out
