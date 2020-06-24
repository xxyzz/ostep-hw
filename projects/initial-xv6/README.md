# Intro To Kernel Hacking

```
// guide and test scripts
$ git clone git@github.com:remzi-arpacidusseau/ostep-projects.git

// xv6 source code
$ git clone git@github.com:mit-pdos/xv6-public.git

// install dependencies
$ brew install gdb x86_64-elf-gcc qemu

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

[What's Cross-Compiler(x86\_64-elf-gcc) and why I need it?](https://wiki.osdev.org/GCC_Cross-Compiler#Introduction)

[ELF](https://wiki.osdev.org/ELF)

[LD](https://sourceware.org/binutils/docs/ld/)

[F.2 Having the Linker Relocate Stabs in ELF](https://sourceware.org/gdb/onlinedocs/stabs.html#ELF-Linker-Relocation)

[DWARF Debugging Information Format](http://dwarfstd.org/doc/DWARF5.pdf)

Debug information are inside `.debug` sections, there are no `.stab` and `.stabstr` sections in input files.

The `src` folder needs all the source files of xv6 to run the test, I only push the modifiled files in here and mark the changed part with comments.

## Debug xv6

```
$ make qemu-nox-gdb
$ gdb kernel
(gdb) b function_name
(gdb) b filename:line_number
(gdb) c
```
