# Common Concurrency Problems

## Homework (Code)

This homework lets you explore some real code that deadlocks (or avoids deadlock). The different versions of code correspond to different approaches to avoiding deadlock in a simplified `vector_add()` routine. See the README for details on these programs and their common substrate.

### Questions

1. First let’s make sure you understand how the programs generally work, and some of the key options. Study the code in `vector-deadlock.c`, as well as in `main-common.c` and related files.

    Now, run `./vector-deadlock -n 2 -l 1 -v`, which instantiates two threads (`-n 2`), each of which does one vector add (`-l 1`), and does so in verbose mode (`-v`). Make sure you understand the output. How does the output change from run to run?

    The order of threads might change.

2. Now add the `-d` flag, and change the number of loops(`-l`) from 1 to higher numbers. What happens? Does the code (always) deadlock?

    ```
    $ ./vector-deadlock -n 2 -l 100000 -v -d
    ```

    Not always, but sometimes.

3. How does changing the number of threads (`-n`) change the outcome of the program? Are there any values of `-n` that ensure no deadlock occurs?

    0, 1

4. Now examine the code in `vector-global-order.c`. First, make sure you understand what the code is trying to do; do you understand why the code avoids deadlock? Also, why is there a special case in this `vector_add()` routine when the source and destination vectors are the same?

    Shouldn't lock the same lock twice.

5. Now run the code with the following flags: `-t -n 2 -l 100000 -d`. How long does the code take to complete? How does the total time change when you increase the number of loops, or the number of threads?

    ```
    $ ./vector-global-order -t -n 2 -l 100000 -d
    Time: 0.02 seconds

    $ ./vector-global-order -t -n 2 -l 200000 -d
    Time: 0.05 seconds

    $ ./vector-global-order -t -n 4 -l 50000 -d
    Time: 0.12 seconds
    ```

6. What happens if you turn on the parallelism flag (`-p`)? How much would you expect performance to change when each thread is working on adding different vectors (which is what `-p` enables) versus working on the same ones?

    Now threads don't need to wait the same two locks.

    ```
    $ ./vector-global-order -t -n 2 -l 100000 -d -p
    Time: 0.01 seconds

    $ ./vector-global-order -t -n 2 -l 200000 -d -p
    Time: 0.02 seconds

    $ ./vector-global-order -t -n 4 -l 50000 -d -p
    Time: 0.01 seconds
    ```

7. Now let’s study `vector-try-wait.c`. First make sure you understand the code. Is the first call to `pthread_mutex_trylock()` really needed? Now run the code. How fast does it run compared to the global order approach? How does the number of retries, as counted by the code, change as the number of threads increases?

    No.

    ```
    $ ./vector-try-wait -t -n 2 -l 100000 -d
    Retries: 2173947
    Time: 0.32 seconds

    $ ./vector-try-wait -t -n 2 -l 200000 -d
    Retries: 3775065
    Time: 0.53 seconds

    $ ./vector-try-wait -t -n 4 -l 50000 -d
    Retries: 1341960
    Time: 0.38 seconds

    $ ./vector-try-wait -t -n 2 -l 100000 -d -p
    Retries: 0
    Time: 0.01 seconds

    $ ./vector-try-wait -t -n 2 -l 200000 -d -p
    Retries: 0
    Time: 0.01 seconds

    $ ./vector-try-wait -t -n 4 -l 50000 -d -p
    Retries: 0
    Time: 0.01 seconds
    ```

8. Now let’s look at `vector-avoid-hold-and-wait.c`. What is the main problem with this approach? How does its performance compare to the other versions, when running both with -p and without it?

    Only one thread is allowed to add at one time.
    
    ```
    $ ./vector-avoid-hold-and-wait -t -n 2 -l 100000 -d
    Time: 0.03 seconds

    $ ./vector-avoid-hold-and-wait -t -n 2 -l 200000 -d
    Time: 0.06 seconds

    $ ./vector-avoid-hold-and-wait -t -n 4 -l 50000 -d
    Time: 0.66 seconds

    $ ./vector-avoid-hold-and-wait -t -n 2 -l 100000 -d -p
    Time: 0.04 seconds

    $ ./vector-avoid-hold-and-wait -t -n 2 -l 200000 -d -p
    Time: 0.07 seconds

    $ ./vector-avoid-hold-and-wait -t -n 4 -l 50000 -d -p
    Time: 0.04 seconds
    ```

9. Finally, let’s look at `vector-nolock.c`. This version doesn’t use locks at all; does it provide the exact same semantics as the other versions? Why or why not?

    It uses fetch and add instead of lock.

10. Now compare its performance to the other versions, both when threads are working on the same two vectors (no `-p`) and when each thread is working on separate vectors (`-p`). How does this no-lock version perform?

    ```
    $ ./vector-nolock -t -n 2 -l 100000 -d
    Time: 0.39 seconds

    $ ./vector-nolock -t -n 2 -l 200000 -d
    Time: 0.83 seconds

    $ ./vector-nolock -t -n 4 -l 50000 -d
    Time: 0.34 seconds

    $ ./vector-nolock -t -n 2 -l 100000 -d -p
    Time: 0.06 seconds

    $ ./vector-nolock -t -n 2 -l 200000 -d -p
    Time: 0.11 seconds

    $ ./vector-nolock -t -n 4 -l 50000 -d -p
    Time: 0.04 seconds
    ```
