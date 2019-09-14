# The Abstraction: The Process

## Homework (Simulation)

This program, `process-run.py`, allows you to see how process states change as programs run and either use the CPU (e.g., perform an addin struction) or do I/O (e.g., send a request to a disk and wait for it to complete). See the README for details.

### Questions

1. Run `process-run.py` with the following flags: `-l 5:100,5:100`. What should the CPU utilization be (e.g., the percent of time the CPU is in use?) Why do you know this? Use the `-c` and `-p` flags tosee if you were right.

    100%, there is no IO process.

2. Now run with these flags: `./process-run.py -l 4:100,1:0`.These flags specify one process with 4 instructions (all to use the CPU), and one that simply issues an I/O and waits for it to be done.How long does it take to complete both processes? Use `-c` and `-p` to find out if you were right.

    4(process 0) + 5(process 1 IO) + 1 = 10

    ```
    $ ./process-run.py -l 4:100,1:0 -c -p
    Time     PID: 0     PID: 1        CPU        IOs 
    1       RUN:cpu      READY          1            
    2       RUN:cpu      READY          1            
    3       RUN:cpu      READY          1            
    4       RUN:cpu      READY          1            
    5          DONE     RUN:io          1            
    6          DONE    WAITING                     1 
    7          DONE    WAITING                     1 
    8          DONE    WAITING                     1 
    9          DONE    WAITING                     1 
    10*        DONE       DONE                       

    Stats: Total Time 10
    Stats: CPU Busy 5 (50.00%)
    Stats: IO Busy  4 (40.00%)
    ```

3. Switch the order of the processes: `-l 1:0,4:100`. What happens now? Does switching the order matter? Why? (As always, use `-c` and `-p` to see if you were right)

    Now process 1 runs when process 0 is waiting for IO completes.

    ```
    $ ./process-run.py -l 1:0,4:100  -c -p
    Time     PID: 0     PID: 1        CPU        IOs 
    1        RUN:io      READY          1            
    2       WAITING    RUN:cpu          1          1 
    3       WAITING    RUN:cpu          1          1 
    4       WAITING    RUN:cpu          1          1 
    5       WAITING    RUN:cpu          1          1 
    6*         DONE       DONE                       

    Stats: Total Time 6
    Stats: CPU Busy 5 (83.33%)
    Stats: IO Busy  4 (66.67%)
    ```

4. We’ll now explore some of the other flags. One important flag is `-S`, which determines how the system reacts when a process issues an I/O. With the flag set to `SWITCH_ON_END`, the systemwill NOT switch to another process while one is doing I/O, instead waiting until the process is completely finished. What happens when you run the following two processes (`-l 1:0,4:100 -c -S SWITCH_ON_END`), one doing I/O and the other doing CPU work?

    Proecss 1 will not run when process 0 is waiting for IO.

5. Now, run the same processes, but with the switching behavior set to switch to another process whenever one is WAITING for I/O (`-l 1:0,4:100 -c -S SWITCH_ON_IO`). What happens now? Use `-c` and `-p` to confirm that you are right.

6. One other important behavior is what to do when an I/O completes. With `-I IO_RUN_LATER`, when an I/O completes, the process that issued it is not necessarily run right away; rather, whatever was running at the time keeps running. What happens when you run this combination of processes? (Run `./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_LATER -c -p`) Are system resources being effectively utilized?

    Process 0 runs the first IO then waits other process done to runs the remain IOs. No.

7. Now run the same processes, but with `-I IO_RUN_IMMEDIATE` set, which immediately runs the process that issued the I/O. How does this behavior differ? Why might running a process that just completed an I/O again be a good idea?

    Now other proecss can run when process 0 is waiting IO. More fair and reduce response time. 

8. Now run with some randomly generated processes: `-s 1 -l 3:50,3:50` or `-s 2 -l 3:50,3:50` or `-s 3 -l 3:50,3:50`. See if you can predict how the trace will turn out. What happens when you use the flag `-I IO_RUN_IMMEDIATE` vs. `-I IO_RUN_LATER`? What happens when you use `-S SWITCH_ON_IO` vs. `-S SWITCH_ON_END`?