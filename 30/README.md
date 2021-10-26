# Condition Variables

## Homework (Code)

This homework lets you explore some real code that uses locks and condition variables to implement various forms of the producer/consumer queue discussed in the chapter. You’ll look at the real code, run it in various configurations, and use it to learn about what works and what doesn’t, as well as other intricacies. Read the README for details.

### Questions

1. Our first question focuses on `main-two-cvs-while.c` (the working solution). First, study the code. Do you think you have an understanding of what should happen when you run the program?

2. Run with one producer and one consumer, and have the producer produce a few values. Start with a buffer (size 1), and then increase it. How does the behavior of the code change with larger buffers? (or does it?) What would you predict `num_full` to be with different buffer sizes (e.g., `-m 10`) and different numbers of produced items (e.g.,` -l 100`), when you change the consumer sleep string from default(nosleep)to `-C 0,0,0,0,0,0,1`?

    ```
    $ ./main-two-cvs-while -p 1 -c 1 -m 1 -v
    $ ./main-two-cvs-while -p 1 -c 1 -m 10 -v
    $ ./main-two-cvs-while -p 1 -c 1 -m 1 -l 100 -v
    $ ./main-two-cvs-while -p 1 -c 1 -m 1 -C 0,0,0,0,0,0,1 -v
    $ ./main-two-cvs-while -p 1 -c 1 -m 10 -l 10 -C 0,0,0,0,0,0,1 -v
    ```

3. If possible, run the code on different systems (e.g., a Mac and Linux). Do you see different behavior across these systems?

    ```
    $ ./main-two-cvs-while -p 1 -c 3 -l 6 -v
    ```

    On Linux 5.14.4, most of the time a single consumer gets all the values but this happens occasionally on macOS 11.6.

4. Let’s look at some timings. How long do you think the following execution, with one producer, three consumers, a single-entry shared buffer, and each consumer pausing at point `c3` for a second, will take? `./main-two-cvs-while -p 1 -c 3 -m 1 -C 0,0,0,1,0,0,0:0,0,0,1,0,0,0:0,0,0,1,0,0,0 -l 10 -v -t`

    11 seconds. It sleeps after the thread reacquiring the lock.

5. Now change the size of the shared buffer to 3 (`-m 3`). Will this make any difference in the total time?

    There are 11 c3, so 11 seconds.

6. Now change the location of the sleep to `c6` (this models a consumer taking something off the queue and then doing something with it), again using a single-entry buffer. What time do you predict in this case? `./main-two-cvs-while -p 1 -c 3 -m 1 -C 0,0,0,0,0,0,1:0,0,0,0,0,0,1:0,0,0,0,0,0,1 -l 10 -v -t`

    It call `sleep` 12 times, but only uses 5 seconds. That's because it sleep after releasing the lock so the other threads can move on.

7. Finally, change the buffer size to 3 again (`-m 3`). What time do you predict now?

    13 c6, still 5 seconds.

8. Now let’s look at `main-one-cv-while.c`. Can you configure a sleep string, assuming a single producer, one consumer, and a buffer of size 1, to cause a problem with this code?

    No

9. Now change the number of consumers to two. Can you construct sleep strings for the producer and the consumers so as to cause a problem in the code?

    ```
    // all threads sleeping, like the situation in Figure 30.11
    $ ./main-one-cv-while -c 2 -v -P 0,0,0,0,0,0,1
    ```

10. Now examine `main-two-cvs-if.c`. Can you cause a problem to happen in this code? Again consider the case where there is only one consumer, and then the case where there is more than one.

    One is fine, two might like Figure 30.7: ready at c3, but there is no data when it's running. Always use `while`.

11. Finally, examine `main-two-cvs-while-extra-unlock.c`. What problem arises when you release the lock before doing a put or a get? Can you reliably cause such a problem to happen, given the sleep strings? What bad thing can happen?

    ```
    $ ./main-two-cvs-while-extra-unlock -p 1 -c 2 -m 10 -l 10 -v -C 0,0,0,0,1,0,0:0,0,0,0,0,0,0
    ```

    First consumer only consumes one value.
