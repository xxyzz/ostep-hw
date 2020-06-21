# Intro To Kernel Hacking

```
// guide and test scripts
$ git clone git@github.com:remzi-arpacidusseau/ostep-projects.git

// xv6 source code
$ git clone git@github.com:mit-pdos/xv6-public.git

// install dependencies
$ brew install gdb x86_64-elf-gcc x86_64-elf-binutils

// edit Makefile
// line 32
TOOLPREFIX = x86_64-elf-
// line 75
AS = $(TOOLPREFIX)as
// line 220
CPUS := 1

// edit kernel.ld
// https://github.com/mit-pdos/xv6-public/pull/115
```

[What's Cross-Compiler(i386-elf-gcc) and why I need it?](https://wiki.osdev.org/GCC_Cross-Compiler#Introduction)

The `src` folder needs all the source files of xv6 to run the test, I only push the modifiled files in here and mark the changed part with comments.

## Debug xv6

```
$ make qemu-nox-gdb
$ gdb kernel
(gdb) b function_name
(gdb) b filename:line_number
(gdb) c
```
