# Interlude: Memory API

## Homework (Code)

In this homework, you will gain some familiarity with memory allocation. First, you’ll write some buggy programs (fun!). Then, you’ll use some tools to help you find the bugs you inserted. Then, you will realize how awesome these tools are and use them in the future, thus making yourself more happy and productive. The tools are the debugger (e.g., `gdb`), and a memory-bug detector called `valgrind`[SN05].

### Questions

1. First, write a simple program called `null.c` that creates a pointer to an integer, sets it to `NULL`, and then tries to dereference it. Compile this into an executable called `null`. What happens when you run this program?

    Segmentation fault.

2. Next,compile this program with symbol information included(with the `-g` flag). Doing so let’s put more information into the executable, enabling the debugger to access more useful information about variable names and the like. Run the program under the debugger by typing `gdb null` and then, once `gdb` is running, typing `run`. What does `gdb` show you?

    ```
    Starting program: /home/pi/hw/null.out 

    Program received signal SIGSEGV, Segmentation fault.
    0x000104d4 in main (argc=1, argv=0x7efff6a4) at null.c:7
    7	    printf("%d\n", *x);
    ```

    |Signal | Value | Action |Comment                 |
    | ----- |:-----:|:------:|:----------------------:|
    |SIGSEGV| 11    | Core   |Invalid memory reference|

3. Finally, use the `valgrind` tool on this program. We’ll use the `memcheck` tool that is a part of `valgrind` to analyze what happens. Run this by typing in the following: `valgrind --leak-check=yes null`. What happens when you run this? Can you interpret the output from the tool?

    ```
    ==25687== Invalid read of size 4
    ==25687==    at 0x104D4: main (null.c:7)
    ==25687==  Address 0x0 is not stack'd, malloc'd or (recently) free'd

    ==25687== Process terminating with default action of signal 11 (SIGSEGV)
    ==25687==  Access not within mapped region at address 0x0
    ==25687==    at 0x104D4: main (null.c:7)

    ==25687== HEAP SUMMARY:
    ==25687==     in use at exit: 4 bytes in 1 blocks
    ==25687==   total heap usage: 1 allocs, 0 frees, 4 bytes allocated
    ==25687== 
    ==25687== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
    ==25687==    at 0x4849CE0: calloc (vg_replace_malloc.c:711)
    ==25687==    by 0x104BF: main (null.c:5)
    ```

    `x` is at the address 0x0, it's not belong to the program.

4. Write a simple program that allocates memory using `malloc()` but forgets to free it before exiting. What happens when this program runs? Can you use `gdb` to find any problems with it? How about `valgrind` (again with the `--leak-check=yes` flag)?

    It runs fine.

    No.

    ```
    ==26394== HEAP SUMMARY:
    ==26394==     in use at exit: 4 bytes in 1 blocks
    ==26394==   total heap usage: 2 allocs, 1 frees, 1,028 bytes allocated
    ==26394== 
    ==26394== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
    ==26394==    at 0x4847568: malloc (vg_replace_malloc.c:299)
    ==26394==    by 0x1048B: main (forget_free.c:5)
    ```

5. Write a program that creates an array of integers called `data` of size 100 using `malloc`; then, set `data[100]` to zero. What happens when you run this program? What happens when you run this program using `valgrind`? Is the program correct?

    Nothing happens.

    ```
    ==26677== Invalid write of size 4
    ==26677==    at 0x1086B1: main (size_100.c:6)
    ==26677==  Address 0x52381d0 is 0 bytes after a block of size 400 alloc'd
    ==26677==    at 0x4C330C5: malloc (vg_replace_malloc.c:442)
    ==26677==    by 0x1086A2: main (size_100.c:5)
    ```

    No. `data[100] = 0;` attempts to store the value 0 in the 101st element of the array. However, since the array was allocated for only 100 integers (indices 0 to 99). Accessing the 101st element lead to memory corruption.

6. Create a program that allocates an array of integers (as above), frees them, and then tries to print the value of one of the elements of the array. Does the program run? What happens when you use `valgrind` on it?

    It runs well.

    ```
    ==26852== Invalid read of size 4
    ==26852==    at 0x104D0: main (free_then_print.c:8)
    ==26852==  Address 0x49c6028 is 0 bytes inside a block of size 100 free'd
    ==26852==    at 0x4848B8C: free (vg_replace_malloc.c:530)
    ==26852==    by 0x104CB: main (free_then_print.c:6)
    ==26852==  Block was alloc'd at
    ==26852==    at 0x4847568: malloc (vg_replace_malloc.c:299)
    ==26852==    by 0x104BB: main (free_then_print.c:5)
    ```
7. Now pass a funny value to free (e.g., a pointer in the middle of the array you allocated above). What happens? Do you need tools to find this type of problem?

    ```
    $ ./free_then_print.out
    free(): invalid pointer
    [1]    29093 abort (core dumped)  ./free_then_print.out
    ```

    ```
    ==29041== Invalid free() / delete / delete[] / realloc()
    ==29041==    at 0x4C30D3B: free (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==29041==    by 0x108706: main (free_then_print.c:7)
    ==29041==  Address 0x522d044 is 4 bytes inside a block of size 100 alloc'd
    ==29041==    at 0x4C2FB0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
    ==29041==    by 0x1086F2: main (free_then_print.c:5)
    ```

8. Try out some of the other interfaces to memory allocation. For example, create a simple vector-like data structure and related routines that use `realloc()` to manage the vector. Use an array to store the vectors elements; when a user adds an entry to the vector, use `realloc()` to allocate more space for it. How well does such a vector perform? How does it compare to a linked list? Use `valgrind` to help you find bugs.

    [Dynamic array - Wikipedia](https://en.wikipedia.org/wiki/Dynamic_array#Performance)

9. Spend more time and read about using `gdb` and `valgrind`. Knowing your tools is critical; spend the time and learn how to become an expert debugger in the UNIX and C environment.
