# Flash-based SSDs

## Homework (Simulation)

This section introduces `ssd.py`, a simple SSD simulator you can use to understand better how SSDs work. Read the README for details on how to run the simulator. It is a long README, so boil a cup of tea (caffeinated likely necessary), put on your reading glasses, let the cat curl up on your lap[^1], and get to work.

### Questions

1. The homework will mostly focus on the log-structured SSD, which is simulated with the “-T log” flag. We’ll use the other types of SSDs for comparison. First, run with flags `-T log -s 1 -n 10 -q`. Can you figure out which operations took place? Use `-c` to check your answers (or just use `-C` instead of `-q -c`). Use different values of `-s` to generate different random workloads.

2. Now just show the commands and see if you can figure out the intermediate states of the Flash. Run with flags `-T log -s 2 -n 10 -C` to show each command. Now, determine the state of the Flash between each command; use `-F` to show the states and see if you were right. Use different random seeds to test your burgeoning expertise.

3. Let’s make this problem ever so slightly more interesting by adding the `-r 20` flag. What differences does this cause in the commands? Use `-c` again to check your answers.

    It will create some failed read commands.

4. Performance is determined by the number of erases, programs, and reads (we assume here that trims are free). Run the same workload again as above, but without showing any intermediate states (e.g., `-T log -s 1 -n 10`). Can you estimate how long this workload will take to complete? (default erase time is 1000 microseconds, program time is 40, and read time is 10) Use the `-S` flag to check your answer. You can also change the erase, program, and read times with the `-E`, `-W`, `-R` flags.

    `-T log`: 1000 + 40 * 4 + 10 * 4 = 1200

5. Now, compare performance of the log-structured approach and the (very bad) direct approach (`-T direct` instead of `-T log`). First, estimate how you think the direct approach will perform, then check your answer with the `-S` flag. In general, how much better will the log-structured approach perform than the direct one?

    `-T direct`: 1000 * 4 + 40 * 5 + 10 * 5 = 4250

6. Let us next explore the behavior of the garbage collector. To do so, we have to set the high (`-G`) and low (`-g`) watermarks appropriately. First, let’s observe what happens when you run a larger workload to the log-structured SSD but without any garbage collection. To do this, run with flags `-T log -n 1000` ~~(the high watermark default is 10, so the GC won’t run in this configuration)~~. What do you think will happen? Use `-C` and perhaps `-F` to see.

    ```
    $ ./ssd.py -T log -s 1 -n 1000 -J -C -G 120
    ```

    The disk is full of garbage.

7. To turn on the garbage collector, use lower values. The high watermark (`-G N`) tells the system to start collecting once N blocks have been used; the low watermark (`-g M`) tells the system to stop collecting once there are only M blocks in use. What watermark values do you think will make for a working system? Use `-C` and `-F` to show the commands and intermediate device states and see.

    The high watermark should be the page of each block.

8. One other useful flag is `-J`, which shows what the collector is doing when it runs. Run with flags `-T log -n 1000 -C -J` to see both the commands and the GC behavior. What do you notice about the GC? The final effect of GC, of course, is performance. Use `-S` to look at final statistics; how many extra reads and writes occur due to garbage collection? Compare this to the ideal SSD (`-T ideal`); how much extra reading, writing, and erasing is there due to the nature of Flash? Compare it also to the direct approach; in what way (erases, reads, programs) is the log-structured approach superior?

    ```
    $ ./ssd.py -T log -s 1 -n 1000 -G 10 -g 8 -S
    Erases sum: 83
    Writes sum: 786
    Reads  sum: 692

    $ ./ssd.py -T ideal -s 1 -n 1000 -G 10 -g 8 -S
    Erases sum: 0
    Writes sum: 492
    Reads  sum: 398

    $ ./ssd.py -T direct -s 1 -n 1000 -G 10 -g 8 -S
    Erases sum: 492
    Writes sum: 4218
    Reads  sum: 4537
    ```

9. One last aspect to explore is **workload skew**. Adding skew to the workload changes writes such that more writes occur to some smaller fraction of the logical block space. For example, running with `-K 80/20` makes 80% of the writes go to 20% of the blocks. Pick some different skews and perform many randomly-chosen operations (e.g., `-n 1000`), using first `-T direct` to understand the skew, and then `-T log` to see the impact on a log-structured device. What do you expect will happen? One other small skew control to explore is `-k 100`; by adding this flag to a skewed workload, the first 100 writes are not skewed. The idea is to first create a lot of data, but then only update some of it. What impact might that have upon a garbage collector?

    ```
    $ ./ssd.py -T direct -s 1 -n 1000 -G 10 -g 8 -S -K 80/20
    Erases sum: 515
    Writes sum: 4407
    Reads  sum: 4743

    $ ./ssd.py -T log -s 1 -n 1000 -G 10 -g 8 -S -K 80/20 -J -C
    Erases sum: 64
    Writes sum: 610
    Reads  sum: 493
    gc 25

    $ ./ssd.py -T direct -s 1 -n 1000 -G 10 -g 8 -S -K 80/20 -k 100
    Erases sum: 516
    Writes sum: 4536
    Reads  sum: 4856

    $ ./ssd.py -T log -s 1 -n 1000 -G 10 -g 8 -S -K 80/20 -k 100 -J -C
    Erases sum: 77
    Writes sum: 726
    Reads  sum: 605
    gc 29
    ```

    Fewer erase sum. Garbage collector commands increase each time.

[^1]: Now you might complain, “But I’m a dog person!” To this, we say, too bad! Get a cat, put it on your lap, and do the homework! How else will you learn, if you can’t even follow the most basic of instructions?
