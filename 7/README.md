# Scheduling: Introduction

## Homework (Simulation)

This program, `scheduler.py`, allows you to see how different schedulers perform under scheduling metrics such as response time, turnaround time, and total wait time. See the README for details.

### Questions

1. Compute the response time and turnaround time when running three jobs of length 200 with the SJF and FIFO schedulers.

    ```
    $ ./scheduler.py -p SJF -l 200,200,200 -c
    response time: 0, 200, 400
    turnaround time: 200, 400, 600

    $ ./scheduler.py -p FIFO -l 200,200,200 -c
    response time: 0, 200, 400
    turnaround time: 200, 400, 600
    ```

2. Now do the same but with jobs of different lengths: 100, 200, and 300.

    ```
    $ ./scheduler.py -p SJF -l 100,200,300 -c
    response time: 0, 100, 300
    turnaround time: 100, 300, 600

    $ ./scheduler.py -p FIFO -l 100,200,300 -c
    response time: 0, 100, 300
    turnaround time: 100, 300, 600
    ```

3. Now do the same, but also with the RR scheduler and a time-slice of 1.

    ```
    $ ./scheduler.py -p RR -q 1 -l 100,200,300 -c
    response time: 0, 1, 2
    turnaround time: 298, 499, 600
    ```

4. For what types of work loads does SJF deliver the same turnaround times as FIFO?

    Jobs are in ascending order by length.

5. For what types of workloads and quantum lengths does SJF deliver the same response times as RR?

    Jobs length are same and quantum length equates to the job length.

6. What happens to response time with SJF as job lengths increase? Can you use the simulator to demonstrate the trend?

    Response time will increase.
    ```
    $ ./scheduler.py -p SJF -l 200,200,200 -c
    $ ./scheduler.py -p SJF -l 300,300,300 -c
    $ ./scheduler.py -p SJF -l 400,400,400 -c
    ```

7. What happens to response time with RR as quantum lengths increase? Can you write an equation that gives the worst-case response time, given N jobs?

	1 <= n <= N, q = quantum time, jt = job run time

	Response time will increase when q < jt:\
	The nth job's response time = (n - 1) * q\
	Average response time = (N - 1) * q / 2

	When q >= jt:\
	The nth job's response time = (n - 1) * jt\
	Average response time = (N - 1) * jt / 2
