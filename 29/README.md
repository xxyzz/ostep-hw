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
    // laptop, no limit on CPUs
    Counter value: 1000000
    Time (seconds): 0.019944

    Counter value: 2000000
    Time (seconds): 0.109112

    Counter value: 3000000
    Time (seconds): 0.164640

    Counter value: 4000000
    Time (seconds): 0.229332
    ```

    ```
    // Runs on Raspberry Pi 3 Model B Rev 1.2
    // CPU: ARMv7 rev 4 (v7l) (4) @ 1.2GHz
    // one CPU
    Counter value: 1000000
    Time (seconds): 0.168540

    Counter value: 2000000
    Time (seconds): 0.234996

    Counter value: 3000000
    Time (seconds): 0.353402

    Counter value: 4000000
    Time (seconds): 0.470331

    // two CPUs
    Counter value: 1000000
    Time (seconds): 0.160590

    Counter value: 2000000
    Time (seconds): 0.526839

    Counter value: 3000000
    Time (seconds): 0.793229

    Counter value: 4000000
    Time (seconds): 0.997977

    // four CPUs
    Counter value: 1000000
    Time (seconds): 0.150495

    Counter value: 2000000
    Time (seconds): 0.519921

    Counter value: 3000000
    Time (seconds): 0.649873

    Counter value: 4000000
    Time (seconds): 0.854063
    ```
