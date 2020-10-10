# Scheduling: The Multi-Level Feedback Queue

## Homework (Simulation)

This program, `mlfq.py`, allows you to see how the MLFQ scheduler presented in this chapter behaves. See the README for details.

### Questions

1. Run a few randomly-generated problems with just two jobs and two queues; compute the MLFQ execution trace for each. Make your life easier by limiting the length of each job and turning off I/Os.

    ```
    $ ./mlfq.py -s 0 -n 2 -j 2 -m 20 -M 0 -c
    $ ./mlfq.py -s 1 -n 2 -j 2 -m 20 -M 0 -c
    ```

2. How would you run the scheduler to reproduce each of the examples in the chapter?

    ```
    // Figure 8.2 Long-running Job Over Time
    $ ./mlfq.py -n 3 -q 10 -l 0,200,0 -c

    // Figure 8.3 Along Came An Interactive Job
    $ ./mlfq.py -n 3 -q 10 -l 0,180,0:100,20,0 -c

    // Figure 8.4 A Mixed I/O-intensive and CPU-intensive Workload
    $ ./mlfq.py -n 3 -q 10 -l 0,175,0:50,25,1 -i 5 -S -c

    // Figure 8.5 without priority boost
    $ ./mlfq.py -n 3 -q 10 -l 0,120,0:100,50,1:100,50,1 -i 1 -S -c

    // Figure 8.5 with priority boost
    $ ./mlfq.py -n 3 -q 10 -l 0,120,0:100,50,1:100,50,1 -i 1 -S -B 50 -c

    // Figure 8.6 without gaming tolerance
    $ ./mlfq.py -n 3 -q 10 -i 1 -S -l 0,200,0:80,100,9 -c

    // Figure 8.6 with gaming tolerance
    $ ./mlfq.py -n 3 -q 10 -i 1 -l 0,200,0:80,100,9 -c

    // Figure 8.7 Lower Priority, Longer Quanta
    $ ./mlfq.py -n 3 -a 2 -Q 10,20,40 -l 0,200,0:0,200,0 -c
    ```

3. How would you configure the scheduler parameters to behave just like a round-robin scheduler?

    time slice <= (max job length / jobs number)

4. Craft a workload with two jobs and scheduler parameters so that one job takes advantage of the older Rules 4a and 4b (turned on with the `-S` flag) to game the scheduler and obtain 99% of the CPU over a particular time interval.

    ```
    $ ./mlfq.py -n 3 -q 10 -l 0,50,0:0,50,9 -i 1 -S -c
    ```

5. Given a system with a quantum length of 10ms in its highest queue, how often would you have to boost jobs back to the highest priority level (with the `-B` flag) in order to guarantee that a single long-running (and potentially-starving) job gets at least 5% of the CPU?

    200ms

    ```
    $ ./mlfq.py -n 3 -q 10 -l 0,200,0:0,200,1:0,200,1 -i 1 -S -B 200 -c
    ```

6. One question that arises in scheduling is which end of a queue to add a job that just finished I/O; the -I flag changes this behavior for this scheduling simulator. Play around with some workloads and see if you can see the effect of this flag.

    ```
    $ ./mlfq.py -n 2 -q 10 -l 0,50,0:0,50,11 -i 1 -S -c
    $ ./mlfq.py -n 2 -q 10 -l 0,50,0:0,50,11 -i 1 -S -I -c
    ```
