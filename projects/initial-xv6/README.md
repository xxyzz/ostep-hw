# Intro To Kernel Hacking

```
// guide and test scripts
$ git clone git@github.com:remzi-arpacidusseau/ostep-projects.git

// xv6 source code
$ git clone git@github.com:mit-pdos/xv6-public.git

// install dependencies
$ brew install gdb x86_64-elf-gcc qemu
```

Edit Makefile:
```diff
@@ -29,7 +29,7 @@ OBJS = \
 	vm.o\
 
 # Cross-compiling (e.g., on Mac OS X)
-# TOOLPREFIX = i386-jos-elf
+TOOLPREFIX = x86_64-elf-

@@ -217,7 +217,7 @@ QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
 	then echo "-gdb tcp::$(GDBPORT)"; \
 	else echo "-s -p $(GDBPORT)"; fi)
 ifndef CPUS
-CPUS := 2
+CPUS := 1
```

[What's Cross-Compiler(x86\_64-elf-gcc) and why I need it?](https://wiki.osdev.org/GCC_Cross-Compiler#Introduction)

[ELF](https://wiki.osdev.org/ELF)

[LD](https://sourceware.org/binutils/docs/ld/)

[F.2 Having the Linker Relocate Stabs in ELF](https://sourceware.org/gdb/onlinedocs/stabs.html#ELF-Linker-Relocation)

[DWARF Debugging Information Format](http://dwarfstd.org/doc/DWARF5.pdf)

Debug information are inside `.debug` sections, there are no `.stab` and `.stabstr` sections in input files.

The `src` folder needs all the source files of xv6 to run the test, I only push the modifiled files in here and mark the changed part with comments.

## Debug xv6

[GDB to LLDB command map](https://lldb.llvm.org/use/map.html)

```
$ make qemu-nox-gdb
$ gdb
(gdb) b function_name
(gdb) b filename:line_number
(gdb) c
(gdb) bt
```
