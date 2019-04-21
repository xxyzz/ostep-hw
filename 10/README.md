# Multiprocessor Scheduling (Advanced)

## Homework (Simulation)

In this homework, we’ll use `multi.py` to simulate a multi-processor CPU scheduler, and learn about some of its details. Read the related README for more information about the simulator and its options.

### Questions

1. To start things off, let’s learn how to use the simulator to study how to build an effective multi-processor scheduler. The first simulation will run just one job, which has a run-time of 30, and a working-set size of 200. Run this job (called job ’a’ here) on one simulated CPU as follows: `./multi.py -n 1 -L a:30:200`. How long will it take to complete? Turn on the `-c` flag to see a final answer, and the `-t` flag to see a tick-by-tick trace of the job and how it is scheduled.

    30.

2. Now increase the cache size so as to make the job’s working set (size=200) fit into the cache (which, by default, is size=100); for example, run `./multi.py -n 1 -L a:30:200 -M 300`. Can you predict how fast the job will run once it fits in cache? (hint: remember the key parameter of the warm rate, which is set by the -r flag) Check your answer by running with the solve flag (`-c`) enabled.

    10 time units for warming up cache, then run two times faster. 10 + 20/2 = 20.

3. One cool thing about `multi.py` is that you can see more detail about what is going on with different tracing flags. Run the same simulation as above, but this time with time left tracing enabled (`-T`). This flag shows both the job that was scheduled on a CPU at each time step, as well as how much run-time that job has left after each tick has run. What do you notice about how that second column decreases?

    It decrease 1 in the first 10 time units, after that it is 2.

4. Now add one more bit of tracing, to show the status of each CPU cache for each job, with the `-C` flag. For each job, each cache will either show a blank space (if the cache is cold for that job) or a ’w’ (if the cache is warm for that job). At what point does the cache become warm for job ’a’ in this simple example? What happens as you change the warmup time parameter (`-w`) to lower or higher values than the default?

    At 10th time unit.

5. At this point, you should have a good idea of how the simulator works for a single job running on a single CPU. But hey, isn’t this a multi-processor CPU scheduling chapter? Oh yeah! So let’s start working with multiple jobs. Specifically, let’s run the following three jobs on a two-CPU system (i.e., type `./multi.py -n 2 -L a:100:100,b:100:50,c:100:50`) Can you predict how long this will take, given a round-robin centralized scheduler? Use `-c` to see if you were right, and then dive down into details with `-t` to see a step-by-step and then `-C` to see whether caches got warmed effectively for these jobs. What do you notice?

    Caches get cleared every 10 time units, it never run faster, 300/2 = 150.

    ```
    $ ./multi.py -n 2 -L a:100:100,b:100:50,c:100:50 -c -t -T -C
    ```

6. Now we’ll apply some explicit controls to study **cache affinity**, as described in the chapter. To do this, you’ll need the `-A` flag. This flag can be used to limit which CPUs the scheduler can place a particular job upon. In this case, let’s use it to place jobs ’b’ and ’c’ on CPU 1, while restricting ’a’ to CPU 0. This magic is accomplished by typing this `./multi.py -n 2 -L a:100:100,b:100:50,c:100:50 -A a:0,b:1,c:1`; don’t forget to turn on various tracing options to see what is really happening! Can you predict how fast this version will run? Why does it do better? Will other combinations of ’a’, ’b’, and ’c’ onto the two processors run faster or slower?

    a complete at 55th time unit(10 + 90/2), b and c both get cache warmed at 20th time unit, so b and c complete at 110th time unit(20 + 90/2*2). This is the fastest combination.

7. One interesting aspect of caching multiprocessors is the opportunity for better-than-expected speed up of jobs when using multiple CPUs (and their caches) as compared to running jobs on a single processor. Specifically, when you run on *N* CPUs, sometimes you can speed up by more than a facor of *N*, a situation entitled **super-linear speedup**. To experiment with this, use the job description here (`-L a:100:100,b:100:100,c:100:100`) with a small cache (`-M 50`) to create three jobs. Run this on systems with 1, 2, and
3 CPUs (`-n 1, -n 2, -n 3`). Now, do the same, but with a larger per-CPU cache of size 100. What do you notice about performance as the number of CPUs scales? Use `-c` to confirm your guesses, and other tracing flags to dive even deeper.

    ```
    $ ./multi.py -n 1 -L a:100:100,b:100:100,c:100:100 -M 50 -c -t -T -C    // 300 no cache
    $ ./multi.py -n 2 -L a:100:100,b:100:100,c:100:100 -M 50 -c -t -T -C    // 150 no cache
    $ ./multi.py -n 3 -L a:100:100,b:100:100,c:100:100 -M 50 -c -t -T -C    // 100 no cache

    $ ./multi.py -n 1 -L a:100:100,b:100:100,c:100:100 -M 100 -c -t -T -C   // 300 no cache
    $ ./multi.py -n 2 -L a:100:100,b:100:100,c:100:100 -M 100 -c -t -T -C   // 150 no cache
    $ ./multi.py -n 3 -L a:100:100,b:100:100,c:100:100 -M 100 -c -t -T -C   // 55, 10 + 90/2
    ```

8. One other aspect of the simulator worth studying is the per-CPU scheduling option, the `-p` flag. Run with two CPUs again, and this three job configuration (`-L a:100:100,b:100:50,c:100:50`). How does this option do, as opposed to the hand-controlled affinity limits you put in place above? How does performance change as you alter the ’peek interval’ (`-P`) to lower or higher values? How does this per-CPU approach work as the number of CPUs scales?

    ```
    $ ./multi.py -n 2 -L a:100:100,b:100:100,c:100:100 -p -c -t -T -C    // 100 = 60 + 10 + (100 - 30 - 10)/2
    $ ./multi.py -n 2 -L a:100:100,b:100:100,c:100:100 -p -c -t -T -C -P 10 // 100
    $ ./multi.py -n 2 -L a:100:100,b:100:100,c:100:100 -p -c -t -T -C -P 40 // 115
    $ ./multi.py -n 3 -L a:100:100,b:100:100,c:100:100 -p -c -t -T -C    // 55
    $ ./multi.py -n 4 -L a:100:100,b:100:100,c:100:100 -p -c -t -T -C    // 60
    $ ./multi.py -n 5 -L a:100:100,b:100:100,c:100:100 -p -c -t -T -C    // 55
    ```

9. Finally, feel free to just generate random workloads and see if you can predict their performance on different numbers of processors, cache sizes, and scheduling options. If you do this, you’ll soon be a **multi-processor scheduling master**, which is a pretty awesome thing to be. Good luck!
