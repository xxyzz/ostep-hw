# Locks

## Homework (Simulation)

This program, `x86.py`, allows you to see how different thread interleavings either cause or avoid race conditions. See the README for details on how the program works and answer the questions below.

### Questions

1. Examine `flag.s`. This code “implements” locking with a single memory flag. Can you understand the assembly?

2. When you run with the defaults, does `flag.s` work? Use the `-M` and `-R` flags to trace variables and registers (and turn on `-c` to see their values). Can you predict what value will end up in `flag`?

    ```
    $ ./x86.py -p flag.s -M flag,count -R ax,bx -c
    ```

    0

3. Change the value of the register `%bx` with the `-a` flag(e.g., `-a bx=2,bx=2` if you are running just two threads). What does the code do? How does it change your answer for the question above?

    ```
    $ ./x86.py -p flag.s -M flag,count -R ax,bx -c -a bx=2,bx=2
    ```

    Each thread runs two loops. `flag` is still 0.

4. Set `bx` to a high value for each thread, and then use the `-i` flag to generate different interrupt frequencies; what values lead to a bad outcomes? Which lead to good outcomes?

    ```
    // bad outcomes
    $ ./x86.py -p flag.s -M flag,count -R ax,bx -c -a bx=10,bx=10 -i 1-10,12,13,14,17

    // god outcomes
    $ ./x86.py -p flag.s -M flag,count -R ax,bx -c -a bx=10,bx=10 -i 11,15,16
    ```

5. Now let’s look at the program `test-and-set.s`. First, try to understand the code, which uses the `xchg` instruction to build a simple locking primitive. How is the lock acquire written? How about lock release?

    Test and acquire in one command.

6. Now run the code, changing the value of the interrupt interval (`-i`) again, and making sure to loop for a number of times. Does the code always work as expected? Does it sometimes lead to an inefficient use of the CPU? How could you quantify that?

    It works. Mutex is 0.

7. Use the `-P` flag to generate specific tests of the locking code. For example, run a schedule that grabs the lock in the first thread, but then tries to acquire it in the second. Does the right thing happen? What else should you test?

    ```
    $ ./x86.py -p test-and-set.s -M mutex,count -R ax,bx -c -a bx=10,bx=10 -P 0011
    ```

    Yes. Fairness and performance.

8. Now let’s look at the code in `peterson.s`, which implements Peterson’s algorithm (mentioned in a sidebar in the text). Study the code and see if you can make sense of it.

9. Now run the code with different values of `-i`. What kinds of different behavior do you see? Make sure to set the thread IDs appropriately (using `-a bx=0,bx=1` for example) as the code assumes it.

    ```
    $ ./x86.py -p peterson.s -M count,flag,turn -R ax,cx -a bx=0,bx=1 -c -i 2
    ```

10. Can you control the scheduling (with the `-P` flag) to “prove” that the code works? What are the different cases you should show hold? Think about mutual exclusion and deadlock avoidance.

    ```
    $ ./x86.py -p peterson.s -M count,flag,turn -R ax,cx -a bx=0,bx=1 -c -P 0000011111
    ```
