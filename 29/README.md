# Lock-based Concurrent Data Structures

## Homework (Code)

In this homework, you’ll gain some experience with writing concurrent code and measuring its performance. Learning to build code that performs well is a critical skill and thus gaining a little experience here with it is quite worthwhile.

### Questions

1. We’ll start by redoing the measurements within this chapter. Use the call `gettimeofday()` to measure time within your program. How accurate is this timer? What is the smallest interval it can measure? Gain confidence in its workings, as we will need it in all subsequent questions. You can also look into other timers, such as the cycle counter available on x86 via the `rdtsc` instruction.

    `gettimeofday()` can measure microseconds.

    [How to Benchmark Code Execution Times on Intel®IA-32 and IA-64 Instruction Set Architectures](https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf)

2. Now, build a simple concurrent counter and measure how long it takes to increment the counter many times as the number of threads increases. How many CPUs are available on the system you are using? Does this number impact your measurements at all?

    4 physical CPUs, 8 logical CPUs. More CPUs should be faster, but in this case it's not!

    ```
    // check CPU numbers
    $ cat /proc/cpuinfo
    $ sysctl hw.physicalcpu
    ```

    Or

    ```c
    #include <stdio.h>
    #include <unistd.h>

    int main(int argc, char *argv[]) {
        // logical CPUs
        printf("Number of logical CPUs: %ld", sysconf(_SC_NPROCESSORS_ONLN));
        return 0;
    }
    ```

    Here are my results. The time of one thread is close to the book(0.03 seconds), but the results of more threads from the book are all exceed five seconds. Maybe I was wrong or the book was wrong.

    ```
    $ make && ./simple_counter.out
    // Runs on four Intel(R) Xeon(R) Gold 6140 CPU @ 2.30GHz CPUs
    1 CPUS
    1 threads
    Time (seconds): 0.022267

    2 threads
    Time (seconds): 0.044044

    3 threads
    Time (seconds): 0.066165

    4 threads
    Time (seconds): 0.080288

    2 CPUS
    1 threads
    Time (seconds): 0.024309

    2 threads
    Time (seconds): 0.317053

    3 threads
    Time (seconds): 0.451622

    4 threads
    Time (seconds): 0.721061

    3 CPUS
    1 threads
    Time (seconds): 0.025448

    2 threads
    Time (seconds): 0.261361

    3 threads
    Time (seconds): 0.288686

    4 threads
    Time (seconds): 0.370454

    4 CPUS
    1 threads
    Time (seconds): 0.024341

    2 threads
    Time (seconds): 0.243560

    3 threads
    Time (seconds): 0.258055

    4 threads
    Time (seconds): 0.422996
    ```

3. Next, build a version of the sloppy counter. Once again, measure its performance as the number of threads varies, as well as the threshold. Do the numbers match what you see in the chapter?

    The time of one threshold in the book is more then ten seconds. But the trend is match.

    ```
    $ make && ./sloppy_counter_counter.out
    1 threads, 1 threshold
    Time (seconds): 0.043183

    2 threads, 1 threshold
    Time (seconds): 0.221763

    3 threads, 1 threshold
    Time (seconds): 0.323453

    4 threads, 1 threshold
    Time (seconds): 0.464092

    1 threads, 2 threshold
    Time (seconds): 0.030793

    2 threads, 2 threshold
    Time (seconds): 0.136044

    3 threads, 2 threshold
    Time (seconds): 0.215075

    4 threads, 2 threshold
    Time (seconds): 0.336686

    0 threads, 3 threshold
    Time (seconds): 0.028138

    1 threads, 3 threshold
    Time (seconds): 0.166224

    2 threads, 3 threshold
    Time (seconds): 0.178742

    3 threads, 3 threshold
    Time (seconds): 0.280512

    1 threads, 4 threshold
    Time (seconds): 0.026591

    2 threads, 4 threshold
    Time (seconds): 0.123020

    3 threads, 4 threshold
    Time (seconds): 0.164652

    4 threads, 4 threshold
    Time (seconds): 0.253098

    1 threads, 5 threshold
    Time (seconds): 0.025639

    2 threads, 5 threshold
    Time (seconds): 0.112037

    3 threads, 5 threshold
    Time (seconds): 0.144308

    4 threads, 5 threshold
    Time (seconds): 0.267464
    ```
