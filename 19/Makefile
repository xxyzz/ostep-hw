CC := clang10
SHELL := /usr/local/bin/zsh
OSFLAG := -D $(shell uname -s)

tlb.out: tlb.c
	$(CC) -O0 -o $@ $< -Weverything -pthread $(OSFLAG)
