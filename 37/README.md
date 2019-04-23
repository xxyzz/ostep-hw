# Hard Disk Drives

This homework uses `disk.py` to familiarize you with how a modern hard drive works. It has a lot of different options, and unlike most of the other simulations, has a graphical animator to show you exactly what happens when the disk is in action. See the README for details.

## Questions

1. Compute the seek, rotation, and transfer times for the following sets of requests: `-a 0,-a 6,-a 30,-a 7,30,8`, and finally `-a 10,11,12,13`.

    ```
    $ ./disk.py -a 0 -G
    $ ./disk.py -a 6 -G
    $ ./disk.py -a 30 -G
    $ ./disk.py -a 7,30,8 -G
    $ ./disk.py -a 10,11,12,13 -G
    ```

2. Do the same requests above, but change the seek rate to different values: `-S 2,-S 4,-S 8,-S 10,-S 40,-S 0.1`. How do the times change?

    ```
    $ ./disk.py -a 7,30,8 -G -S 2
    $ ./disk.py -a 7,30,8 -G -S 4
    ```

    Seek time is shorter. Default value is 1.

3. Do the same requests above, but change the rotation rate: `-R 0.1,-R 0.5,-R 0.01`. How do the times change?

    Default value is 1. Rotate time and transfer time are longer.

4. FIFO is not always best, e.g., with the request stream `-a 7,30,8`, what order should the requests be processed in? Run the shortest seek-time first (SSTF) scheduler (`-p SSTF`) on this workload; how long should it take (seek, rotation, transfer) for each request to be served?

    ```
    $ ./disk.py -a 7,30,8 -c -p SSTF
    ```

    FIFO order: 7, 30, 8

    SSTF order: 7, 8, 30

5. Now use the shortest access-time first(SATF) scheduler(`-p SATF`). Does it make any difference for `-a 7,30,8` workload? Find a set of requests where SATF outperforms SSTF; more generally, when is SATF better than SSTF?

    No difference.

    ```
    $ ./disk.py -a 31,6 -c -p SATF -S 40 -R 1
    $ ./disk.py -a 31,6 -c -p SSTF -S 40 -R 1
    ```

    When seek time is shorter then rotate time.

6. Here is a request stream to try: `-a 10,11,12,13`. What goes poorly when it runs? Try adding track skew to address this problem (`-o skew`). Given the default seek rate, what should the skew be to maximize performance? What about for different seek rates (e.g., `-S 2, -S 4`)? In general, could you write a formula to figure out the skew?

    ```
    $ ./disk.py -a 10,11,12,13 -c
    $ ./disk.py -a 10,11,12,13 -o 2 -c
    ```

    skew = track-distance(40) / seek-speed / (rotational-space-degrees(360 / 12)  * rotation-speed)
    = 40 / 1 / (30 * 1) ≈ 2

    `-S 2`: 40 / 2 / 30 ≈ 1

    `-S 4`: 40 / 4 / 30 ≈ 1
