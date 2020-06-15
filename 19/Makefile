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

tlb: tlb.c
	$(CC) -O0 -o tlb.out tlb.c -Weverything -pthread $(OSFLAG)
