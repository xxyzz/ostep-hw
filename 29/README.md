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
    1 CPUs                                 
    1 threads
    Time (seconds): 0.022937

    2 threads
    Time (seconds): 0.024302

    3 threads 
    Time (seconds): 0.022126
    
    4 threads
    Time (seconds): 0.021776

    2 CPUs
    1 threads
    Time (seconds): 0.021761

    2 threads
    Time (seconds): 0.131118

    3 threads
    Time (seconds): 0.103083

    4 threads
    Time (seconds): 0.138586

    3 CPUs
    1 threads
    Time (seconds): 0.023613

    2 threads
    Time (seconds): 0.112032

    3 threads
    Time (seconds): 0.100858

    4 threads
    Time (seconds): 0.097561

    4 CPUs
    1 threads
    Time (seconds): 0.021350

    2 threads
    Time (seconds): 0.132612

    3 threads
    Time (seconds): 0.091974

    4 threads
    Time (seconds): 0.091199
    ```

3. Next, build a version of the sloppy counter. Once again, measure its performance as the number of threads varies, as well as the threshold. Do the numbers match what you see in the chapter?

    The time of one threshold in the book is more then ten seconds. But the trend is match.

    ```
    $ make && ./sloppy_counter.out
    // Runs on four Intel(R) Core(TM) i5-8259U CPU @ 2.30GHz CPUs
    1 threads, 1 threshold
    Time (seconds): 0.047187

    2 threads, 1 threshold
    Time (seconds): 0.122691

    3 threads, 1 threshold
    Time (seconds): 0.113234

    4 threads, 1 threshold
    Time (seconds): 0.119592

    1 threads, 2 threshold
    Time (seconds): 0.032644

    2 threads, 2 threshold
    Time (seconds): 0.092127

    3 threads, 2 threshold
    Time (seconds): 0.075995

    4 threads, 2 threshold
    Time (seconds): 0.090719

    1 threads, 3 threshold
    Time (seconds): 0.031007

    2 threads, 3 threshold
    Time (seconds): 0.081507

    3 threads, 3 threshold
    Time (seconds): 0.061782

    4 threads, 3 threshold
    Time (seconds): 0.077193

    1 threads, 4 threshold
    Time (seconds): 0.029097

    2 threads, 4 threshold
    Time (seconds): 0.067766

    3 threads, 4 threshold
    Time (seconds): 0.057847

    4 threads, 4 threshold
    Time (seconds): 0.068740

    1 threads, 5 threshold
    Time (seconds): 0.028153

    2 threads, 5 threshold
    Time (seconds): 0.063899

    3 threads, 5 threshold
    Time (seconds): 0.052178

    4 threads, 5 threshold
    Time (seconds): 0.065875
    ```

4. Build a version of a linked list that uses hand-over-hand locking[MS04](https://www.cs.tau.ac.il/~shanir/concurrent-data-structures.pdf), as cited in the chapter. You should read the paper first to understand how it works, and then implement it. Measure its performance. When does a hand-over-hand list work better than a standard list as shown in the chapter?

5. Pick your favorite interesting data structure, such as a B-tree or other slightly more interested structure. Implement it, and start with a simple locking strategy such as a single lock. Measure its performance as the number of concurrent threads increases.

    Translate from [algs4 BTree](https://github.com/kevin-wayne/algs4/blob/master/src/main/java/edu/princeton/cs/algs4/BTree.java)
