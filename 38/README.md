# Redundant Arrays of Inexpensive Disks (RAIDs)

This section introduces `raid.py`, a simple RAID simulator you can use to shore up your knowledge of how RAID systems work. See the README for details.

## Questions

1. Use the simulator to perform some basic RAID mapping tests. Run with different levels (0, 1, 4, 5) and see if you can figure out the mappings of a set of requests. For RAID-5, see if you can figure out the difference between left-symmetric and left-asymmetric layouts. Use some different random seeds to generate different problems than above.

    ```
    $ ./raid.py -L 5 -5 LS -c -W seq
    $ ./raid.py -L 5 -5 LA -c -W seq

    left-symmetric    left-asymmetric
    0 1 2 P           0 1 2 P
    4 5 P 3           3 4 P 5
    8 P 6 7           6 P 7 8
    ```

2. Do the same as the first problem, but this time vary the chunk size with `-C`. How does chunk size change the mappings?

    ```
    $ ./raid.py -L 5 -5 LS -c -W seq -C 8K -n 12

    0  2  4  P
    1  3  5  P
    8 10  P  6
    9 11  P  7
    ```

3. Do the same as above, but use the `-r` flag to reverse the nature of each problem.

    ```
    $ ./raid.py -L 5 -5 LS -W seq -C 8K -n 12 -r
    ```

4. Now use the reverse flag but increase the size of each request with the `-S` flag. Try specifying sizes of 8k, 12k, and 16k, while varying the RAID level. What happens to the underlying I/O pattern when the size of the request increases? Make sure to try this with the sequential workload too (`-W sequential`); for what request sizes are RAID-4 and RAID-5 much more I/O efficient?

    ```
    $ ./raid.py -L 4 -S 4k -c -W seq
    $ ./raid.py -L 4 -S 8k -c -W seq
    $ ./raid.py -L 4 -S 12k -c -W seq
    $ ./raid.py -L 4 -S 16k -c -W seq
    $ ./raid.py -L 5 -S 4k -c -W seq
    $ ./raid.py -L 5 -S 8k -c -W seq
    $ ./raid.py -L 5 -S 12k -c -W seq
    $ ./raid.py -L 5 -S 16k -c -W seq
    ```

    16k

5. Use the timing mode of the simulator (`-t`) to estimate the performance of 100 random reads to the RAID, while varying the RAID levels, using 4 disks.

    ```
    $ ./raid.py -L 0 -t -n 100 -c    // 275.7
    $ ./raid.py -L 1 -t -n 100 -c    // 278.7
    $ ./raid.py -L 4 -t -n 100 -c    // 386.1
    $ ./raid.py -L 5 -t -n 100 -c    // 276.5
    ```

6. Do the same as above, but increase the number of disks. How does the performance of each RAID level scale as the number of disks increases?

    ```
    $ ./raid.py -L 0 -t -n 100 -c -D 8   // 275.7 / 156.5 = 1.76
    $ ./raid.py -L 1 -t -n 100 -c -D 8   // 278.7 / 167.8 = 1.66
    $ ./raid.py -L 4 -t -n 100 -c -D 8   // 386.1 / 165.0 = 2.34
    $ ./raid.py -L 5 -t -n 100 -c -D 8   // 276.5 / 158.6 = 1.74
    ```

7. Do the same as above, but use all writes (`-w 100`) instead of reads. How does the performance of each RAID level scale now? Can you do a rough estimate of the time it will take to complete the workload of 100 random writes?

    ```
    $ ./raid.py -L 0 -t -n 100 -c -w 100       // 275.7    100 * 10 / 4
    $ ./raid.py -L 1 -t -n 100 -c -w 100       // 509.8    100 * 10 / (4 / 2)
    $ ./raid.py -L 4 -t -n 100 -c -w 100       // 982.5
    $ ./raid.py -L 5 -t -n 100 -c -w 100       // 497.4
    $ ./raid.py -L 0 -t -n 100 -c -D 8 -w 100  // 275.7 / 156.5 = 1.76    100 * 10 / 8
    $ ./raid.py -L 1 -t -n 100 -c -D 8 -w 100  // 509.8 / 275.7 = 1.85    100 * 10 / (8 / 2)
    $ ./raid.py -L 4 -t -n 100 -c -D 8 -w 100  // 982.5 / 937.8 = 1.05
    $ ./raid.py -L 5 -t -n 100 -c -D 8 -w 100  // 497.4 / 290.9 = 1.71
    ```

8. Run the timing mode one last time, but this time with a sequential workload(`-W sequential`). How does the performance vary with RAID level, and when doing reads versus writes? How about when varying the size of each request? What size should you write to a RAID when using RAID-4 or RAID-5?

    ```
    $ ./raid.py -L 0 -t -n 100 -c -w 100 -W seq    // 275.7 / 12.5 = 22
    $ ./raid.py -L 1 -t -n 100 -c -w 100 -W seq    // 509.8 / 15 = 34
    $ ./raid.py -L 4 -t -n 100 -c -w 100 -W seq    // 982.5 / 13.4 = 73
    $ ./raid.py -L 5 -t -n 100 -c -w 100 -W seq    // 497.4 / 13.4 = 37
    ```

    12k.
