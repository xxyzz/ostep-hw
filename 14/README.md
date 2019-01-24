# Interlude: Memory API

## Homework (Code)

In this homework, you will gain some familiarity with memory allocation. First, you’ll write some buggy programs (fun!). Then, you’ll use some tools to help you find the bugs you inserted. Then, you will realize how awesome these tools are and use them in the future, thus making yourself more happy and productive. The tools are the debugger (e.g., `gdb`), and a memory-bug detector called `valgrind`[SN05].

### Questions

1. First, write a simple program called `null.c` that creates a pointer to an integer, sets it to `NULL`, and then tries to dereference it. Compile this into an executable called `null`. What happens when you run this program?

    Segmentation fault.

2. Next,compile this program with symbol information included(with the `-g` flag). Doing so let’s put more information into the executable, enabling the debugger to access more useful information about variable names and the like. Run the program under the debugger by typing `gdb null` and then, once `gdb` is running, typing `run`. What does `gdb` show you?

    ```
    Program received signal SIGSEGV, Segmentation fault.
    0x000104d4 in main (argc=1, argv=0x7efff674) at null.c:7
    7           printf("%d\n", *x);
    ```

