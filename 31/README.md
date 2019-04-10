# Semaphores

## Homework (Code)

In this homework, we’ll use semaphores to solve some well-known concurrency problems. Many of these are taken from Downey’s excellent [“Little Book of Semaphores”](https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf), which does a good job of pulling together a number of classic problems as well as introducing a few new variants; interested readers should check out the Little Book for more fun.

Each of the following questions provides a code skeleton; your job is to fill in the code to make it work given semaphores. On Linux, you will be using native semaphores; ~~on a Mac (where there is no semaphore support), you’ll have to first build an implementation (using locks and condition variables, as described in the chapter)~~. Good luck!

### Questions

1. The first problem is just to implement and test a solution to the **fork/join problem**, as described in the text. Even though this solution is described in the text, the act of typing it in on your own is worthwhile; even Bach would rewrite Vivaldi, allowing one soon-to-be master to learn from an existing one. See `fork-join.c` for details. Add the call `sleep(1)` to the child to ensure it is working.

    macOS does support semaphore, but `sem_init()` is deprecated, I use `sem_open` here.

2. Let’s now generalize this a bit by investigating the **rendezvous problem**. The problem is as follows: you have two threads, each of which are about to enter the rendezvous point in the code. Neither should exit this part of the code before the other enters it. Consider using two semaphores for this task, and see `rendezvous.c` for details.
