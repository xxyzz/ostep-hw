# Paging: Faster Translations (TLBs)

## Homework (Measurement)

### Questions

1. For timing, you’ll need to use a timer (e.g., `gettimeofday()`). How precise is such a timer? How long does an operation have to take in order for you to time it precisely? (this will help determine how many times, in a loop, you’ll have to repeat a page access in order to time it successfully)

    `CLOCK_PROCESS_CPUTIME_ID` has resolution of 1 nanosecond on Linux, 1000 nanoseconds on macOS.

2. Write the program, called `tlb.c`, that can roughly measure the cost of accessing each page. Inputs to the program should be: the number of pages to touch and the number of trials.

3. Now write a script in your favorite scripting language (bash?) to run this program, while varying the number of pages accessed from 1 up to a few thousand, perhaps incrementing by a factor of two per iteration. Run the script on different machines and gather some data. How many trials are needed to get reliable measurements?

    100000 trials.

4. Next, graph the results, making a graph that looks similar to the one above. Use a good tool like `ploticus` or even `zplot`. Visualization usually makes the data much easier to digest; why do you think that is?

    ```
    $ make
    $ python plot.py 14 100000 --single_cpu
    ```

    ![Linux_100000_single](https://user-images.githubusercontent.com/21101839/171332561-c1486739-5f25-4490-b55d-c69702a22a60.png)
    ![Darwin_100000](https://user-images.githubusercontent.com/21101839/171332431-e321dbd2-c650-48d4-a14f-77d3d2db03e2.png)

    >Which organ provides the greatest bandwidth in terms of its access to the human brain? Obviously, the eyeball.
    >
    >-- *Dealers of Lightning, Chapter 1*

5. One thing to watch out for is compiler optimization. Compilers do all sorts of clever things, including removing loops which increment values that no other part of the program subsequently uses. How can you ensure the compiler does not remove the main loop above from your TLB size estimator?

    Using gcc's optimize option `gcc -O0` to disable optimization. This is the default setting.

6. Another thing to watch out for is the fact that most systems today ship with multiple CPUs, and each CPU, of course, has its own TLB hierarchy. To really get good measurements, you have to run your code on just one CPU, instead of letting the scheduler bounce it from one CPU to the next. How can you do that? (hint: look up “pinning a thread” on Google for some clues) What will happen if you don’t do this, and the code moves from one CPU to the other?

    Use `sched_setaffinity(2)`, `pthread_setaffinity_np(3)`, `taskset(1)` or `sudo systemd-run -p AllowedCPUs=0 ./tlb.out` on Linux, `cpuset_setaffinity(2)` or `cpuset(1)` on FreeBSD.

    Or use `hwloc-bind package:0.pu:0 -- ./tlb.out`.

7. Another issue that might arise relates to initialization. If you don’t initialize the array `a` above before accessing it, the first time you access it will be very expensive, due to initial access costs such as demand zeroing. Will this affect your code and its timing? What can you do to counterbalance these potential costs?

    Use `calloc(3)` to initialize array then measure time.
