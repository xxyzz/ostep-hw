# Interlude: Thread API

## Homework (Code)

In this section, we’ll write some simple multi-threaded programs and use a specific tool, called `helgrind`, to find problems in these programs.

Read the README in the homework download for details on how to build the programs and run `helgrind`.

### Questions

1. First build `main-race.c`. Examine the code so you can see the (hopefully obvious) data race in the code. Now run `helgrind` (by typing `valgrind --tool=helgrind main-race`) to see how it reports the race. Does it point to the right lines of code? What other information does it give to you?

    Yes. The conflict address and size.

2. What happens when you remove one of the offending lines of code? Now add a lock around one of the updates to the shared variable, and then around both. What does helgrind report in each of these cases?

    No error.

    Possible data race.

    No error.

3. Now let’s look at `main-deadlock.c`. Examine the code. This code has a problem known as **deadlock** (which we discuss in much more depth in a forthcoming chapter). Can you see what problem it might have?

    Two thread lock each other.

4. Now run `helgrind` on this code. What does `helgrind` report?

    ```
    $ valgrind --tool=helgrind ./main-deadlock
    lock order violated
    ```

5. Now run `helgrind` on `main-deadlock-global.c`. Examine the code; does it have the same problem that `main-deadlock.c` has? Should `helgrind` be reporting the same error? What does this tell you about tools like `helgrind`?

    Yes. No. It's not prefect.

6. Let’s next look at `main-signal.c`. This code uses a variable (`done`) to signal that the child is done and that the parent can now continue. Why is this code inefficient? (what does the parent end up spending its time doing, particularly if the child thread takes a long time to complete?)

    The parent falls in the loop and do nothing.

7. Now run `helgrind` on this program. What does it report? Is the code correct?

    Possible data race of write/read `done` and `printf()`. No.

8. Now look at a slightly modified version of the code, which is found in `main-signal-cv.c`. This version uses a condition variable to do the signaling (and associated lock). Why is this code preferred to the previous version? Is it correctness, or performance, or both?

    Both.

9. Once again run `helgrind` on `main-signal-cv`. Does it report any errors?

    No error!
