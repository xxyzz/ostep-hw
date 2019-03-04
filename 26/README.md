# Concurrency: An Introduction

## Homework (Simulation)

This program, `x86.py`, allows you to see how different thread inter-leavings either cause or avoid race conditions. See the README for details on how the program works, then answer the questions below.

### Questions

1. Let’s examine a simple program, “loop.s”. First, just read and understand it. Then, run it with these arguments(`./x86.py -p loop.s -t 1 -i 100 -R dx`) This specifies a single thread, an interrupt every 100 instructions, and tracing of register `%dx`. What will `%dx` be during the run? Use the `-c` flag to check your answers; the answers, on the left, show the value of the register (or memory value) *after* the instruction on the right has run.

    ```
    $ ./x86.py -p loop.s -t 1 -i 100 -R dx -c
    dx          Thread 0         
     0   
    -1   1000 sub  $1,%dx
    -1   1001 test $0,%dx
    -1   1002 jgte .top
    -1   1003 halt
    ```

2. Same code, different flags: (`./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx`)This specifies two threads, and initializes each `%dx` to 3. What values will `%dx` see? Run with `-c` to check. Does the presence of multiple threads affect your calculations? Is there a race in this code?

    Three. No. No.

3. Run this:`./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx` This makes the interrupt interval small/random; use different seeds (`-s`) to see different interleavings. Does the interrupt frequency change anything?

4. Now, a different program, `looping-race-nolock.s`, which accesses a shared variable located at address 2000; we’ll call this variable `value`. Run it with a single thread to confirm your understanding: `./x86.py -p looping-race-nolock.s -t 1 -M 2000` What is `value`(i.e.,at memory address 2000) throughout the run? Use `-c` to check.

    ```
    $ ./x86.py -p looping-race-nolock.s -t 1 -M 2000 -c
    2000          Thread 0         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   1006 halt
    ```

5. Run with multiple iterations/threads: `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000` Why does each thread loop three times? What is final value of `value`?

    Because bx is initialized to three. Six.

6. Run with random interrupt intervals: `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0` with different seeds (`-s 1`, `-s 2`, etc.) Can you tell by looking at the thread interleaving what the final value of `value` will be? Does the timing of the interrupt matter? Where can it safely occur? Where not? In other words, where is the critical section exactly?

    Whether increased ax is saved to `value` before get reset.

7. Now examine fixed interrupt intervals: `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1` What will the final value of the shared variable `value` be? What about when you change `-i 2`, `-i 3`, etc.? For
which interrupt intervals does the program give the “correct” answer?

    One. One. Two. Intervals >= 3.

8. Run the same for more loops (e.g., set` -a bx=100`). What interrupt intervals (`-i`) lead to a correct outcome? Which intervals are surprising?

    intervals >= 597, == 3 * x (x = 1, 2, 3...)

9. One last program: `wait-for-me.s`. Run: `./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000` This sets the `%ax` register to 1 for thread 0, and 0 for thread 1, and watches `%ax` and memory location 2000. How should the code behave? How is the value at location 2000 being used by the threads? What will its final value be?

    Thread 0 sets memory 2000 to 1 then halt. Thread 1 keeps running in a loop until memory 2000 is 1.

10. Now switch the inputs: `./x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000` How do the threads behave? What is thread 0 doing? How would changing the interrupt interval (e.g., `-i 1000`, or perhaps to use random intervals) change the trace outcome? Is the program efficiently using the CPU?

    The threads swapped. Now thread 0 keeps running in loop waiting thread 1 changes memory 2000 to 1.

    No.
