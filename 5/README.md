# Interlude: Process API

## Homework (Simulation)

This simulation homework focuses on `fork.py`, a simple process creation simulator that shows how processes are related in a single “familial” tree. Read the relevant README for details about how to run the simulator.

### Questions

1. Run `./fork.py -s 10` and see which actions are taken. Can you predict what the process tree looks like at each step? Use the `-c` flag to check your answers. Try some different random seeds(`-s`) or add more actions(`-a`) to get the hang of it.

2. One control the simulator gives you is the `fork_percentage`, controlled by the `-f` flag. The higher it is, the more likely the next action is a fork; the lower it is, the more likely the action is an exit. Run the simulator with a large number of actions (e.g., `-a 100`) and vary the fork percentage from 0.1 to 0.9. What do youthink the resulting final process trees will look like as the percentage changes? Check your answer with `-c`.

0.1: flat tree, a has just a few generations of children.

0.9: tree has more depth, more children of a's children.

3. Now, switch the output by using the `-t` flag (e.g., run `./fork.py -t`). Given a set of process trees, can you tell which actions were taken?

4. One interesting thing to note is what happens when a child exits; what happens to its children in the process tree? To study this, let’s create a specific example: `./fork.py -A a+b,b+c,c+d,c+e,c-`. This example has process ’a’ create ’b’, which in turn creates ’c’,which then creates ’d’ and ’e’. However, then, ’c’ exits. What do you think the process tree should like after the exit? What if you use the `-R` flag? Learn more about what happens to orphaned processes on your own to add more context.

I though `d` and `e` will become `b`'s children but in fact they are `a`'s. After enabling local reparent option(`-R`), they are `b`'s children.

5. One last flag to explore is the `-F` flag, which skips intermediate steps and only asks to fill in the final process tree. Run `./fork.py -F` and see if you can write down the final tree by looking at the series of actions generated. Use different random seeds to try this a few times.

6. Finally, use both `-t` and `-F` together. This shows the final process tree, but then asks you to fill in the actions that took place. By looking at the tree, can you determine the exact actions that took place? In which cases can you tell? In which can’t you tell? Try some different random seeds to delve into this question.

Actions are easy to guess but orders are not.

## Homework (Code)

In this homework, you are to gain some familiarity with the process management APIs about which you just read. Don’t worry – it’s even more fun than it sounds! You’ll in general be much better off if you find as much time as you can to write some code, so why not start now?

### Questions

1. Write a program that calls `fork()`. Before calling `fork()`, have the main process access a variable (e.g., `x`) and set its value to something (e.g., `100`). What value is the variable in the child process? What happens to the variable when both the child and parent change the value of `x`?

    - The value is the variable in the child process. 
    
    - They both change the value in their own process.

2. Write a program that opens a file (with the `open()` system call) and then calls `fork()` to create a new process. Can both the child and parent access the file descriptor returned by `open()`? What happens when they are writing to the file concurrently, i.e., at the same time?

    They both can access the file descriptor.

    Most times, the parent writes before the child:

    ```
    file descriptor in parent prosess: 3
    file descriptor in child process: 3

    file contents:
    First line.
    parent writes a line.
    First line.
    child writes a line.
    ```

    Sometimes they overwrite each other(I only get this result on FreeBSD so far):

    ```
    file descriptor in parent prosess: 3
    file descriptor in child process: 3

    file contents:
    First line.
    First line.
    child writes a line.
    parent writes a line.
    ```

    I think it depends on the system scheduler.

3. Write another program using `fork()`. The child process should print “hello”; the parent process should print “goodbye”. You should try to ensure that the child process always prints first; can you do this *without* calling **wait()** in the parent?

    - Using `kill()` to send `SIGCONT` signal to parent process.

4. Write a program that calls `fork()` and then calls some form of `exec()` to
run the program `/bin/ls`. See if you can try all of the variants of `exec()`, including(on Linux)`execl()`, `execle()`, `execlp()`, `execv()`, `execvp()`, and `execvpe()`. Why do you think there are so many variants of the same basic call?

    - [exec (system call): C language prototypes](https://en.wikipedia.org/wiki/Exec_(system_call)#C_language_prototypes)

5. Now write a program that uses `wait()` to wait for the child process to finish in the parent. What does `wait()` return? What happens if you use `wait()` in the child?

    - On success, returns the process ID(`pid_t`) of the terminated child; on error, -1 is returned.

    - Returns -1.

6. Write a slight modification of the previous program,this time using `waitpid()` instead of wait(). When would `waitpid()` be useful?

7. Write a program that creates a child process, and then in the child closes standard output (`STDOUT_FILENO`). What happens if the child calls `printf()` to print some output after closing the descriptor?

    - The result of `printf()` won't show in the terminal.

8. Write a program that creates two children, and connects the standard output of one to the standard input of the other, using the `pipe()` system call.
