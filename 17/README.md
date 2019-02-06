# Free-Space Management

## Homework (Simulation)

The program, `malloc.py`, lets you explore the behavior of a simple free-space allocator as described in the chapter. See the README for details of its basic operation.

### Questions

1. First run with the flags `-n 10 -H 0 -p BEST -s 0` to generate a few random allocations and frees. Can you predict what alloc()/free() will return? Can you guess the state of the free list after each request? What do you notice about the free list over time?

    The memory is chopped up into pieces.

2. How are the results different when using a WORST fit policy to search the freelist(`-p WORST`)? What changes?

    The memory is deviled to more pieces, and searched more elements.

3. What about when using FIRST fit `(-p FIRST`)? What speeds up when you use first fit?

    Searched less elements.

4. For the above questions, how the list is kept ordered can affect the time it takes to find a free location for some of the policies. Use the different free list orderings(`-l ADDRSORT,-l SIZESORT+,-l SIZESORT-`) to see how the policies and the list orderings interact.

    ```
    $ ./malloc.py -p BEST -l SIZESORT+ -c
    $ ./malloc.py -p FIRST -l SIZESORT+ -c
    $ ./malloc.py -p WORST -l SIZESORT- -c
    ```
5. Coalescing of a free list can be quite important. Increase the number of random allocations (say to -n 1000). What happens to larger allocation requests over time? Run with and without coalescing (i.e., without and with the -C flag). What differences in outcome do you see? How big is the free list over time in each case? Does the ordering of the list matter in this case?

    ```
    $ ./malloc.py -n 1000 -r 30 -c
    $ ./malloc.py -n 1000 -r 30 -c -C
    ```

    Without coalescing, larger allocation requests will return NULL and the free list's size is bigger.

    Sort by address is better.

6. What happens when you change the percent allocated fraction `-P` to higher than 50? What happens to allocations as it nears 100? What about as the percent nears 0?

    ```
    $ ./malloc.py -c -n 1000 -P 100
    $ ./malloc.py -c -n 1000 -P 1
    ```

    No more spaces to allocate. All pointers are freed.

7. What kind of specific requests can you make to generate a highly-fragmented free space? Use the -A flag to create fragmented free lists, and see how different policies and options change the organization of the free list.

    ```
    $ ./malloc.py -c -A +20,+20,+20,+20,+20,-0,-1,-2,-3,-4
    $ ./malloc.py -c -A +20,+20,+20,+20,+20,-0,-1,-2,-3,-4 -C
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -l SIZESORT-
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -l SIZESORT- -C
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p FIRST -l SIZESORT+
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p FIRST -l SIZESORT+ -C
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p FIRST -l SIZESORT-
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p FIRST -l SIZESORT- -C
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p WORST -l SIZESORT+
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p WORST -l SIZESORT+ -C
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p WORST -l SIZESORT-
    $ ./malloc.py -c -A +10,-0,+20,-1,+30,-2,+40,-3 -p WORST -l SIZESORT- -C
    ```
