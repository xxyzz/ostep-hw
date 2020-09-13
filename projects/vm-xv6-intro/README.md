# Intro To xv6 Virtual Memory

## Null-pointer Dereference

Before the patch, the `null` program:

```c
#include "types.h"
#include "user.h"

int main() {
  char *n = 0;
  printf(1, "%x\n", *n);
  exit();
}
```

get this output:

```
pid 3 null: trap 6 err 0 on cpu 0 eip 0x7 addr 0x0--kill proc
```

it's not a page fault(illegal opcode, what the hell?). Increase `n` before using it:

```c
n++;
printf(1, "%x\n", *n);
```

then it prints out `4c`, check where it come from:

```
$ x86_64-elf-objdump -d null.o

null.o:     file format elf32-i386


Disassembly of section .text.startup:

00000000 <main>:
   0:   8d 4c 24 04             lea    0x4(%esp),%ecx
   4:   83 e4 f0                and    $0xfffffff0,%esp
   7:   ff 71 fc                pushl  -0x4(%ecx)
   a:   55                      push   %ebp
   b:   89 e5                   mov    %esp,%ebp
   d:   51                      push   %ecx
   e:   83 ec 08                sub    $0x8,%esp
  11:   0f be 05 01 00 00 00    movsbl 0x1,%eax
  18:   50                      push   %eax
  19:   68 00 00 00 00          push   $0x0
  1e:   6a 01                   push   $0x1
  20:   e8 fc ff ff ff          call   21 <main+0x21>
  25:   e8 fc ff ff ff          call   26 <main+0x26>
```

xv6 loads the program at address 0. After applying the patch, virtual address starts at PGSIZE(4096). null(0) is not mapped. Now the error should be:

```
pid 3 null: trap 14 err 4 on cpu 0 eip 0x1000 addr 0x0--kill proc
```

```
// traps.h:
#define T_DIVIDE         0      // divide error
#define T_OFLOW          4      // overflow
#define T_ILLOP          6      // illegal opcode
#define T_DEVICE         7      // device not available
#define T_PGFLT         14      // page fault
```
