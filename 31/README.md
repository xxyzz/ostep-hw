# Semaphores

## Homework (Code)

In this homework, we’ll use semaphores to solve some well-known concurrency problems. Many of these are taken from Downey’s excellent [“Little Book of Semaphores”](https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf), which does a good job of pulling together a number of classic problems as well as introducing a few new variants; interested readers should check out the Little Book for more fun.

Each of the following questions provides a code skeleton; your job is to fill in the code to make it work given semaphores. On Linux, you will be using native semaphores; on a Mac (where there is no semaphore support), you’ll have to first build an implementation (using locks and condition variables, as described in the chapter). Good luck!

### Questions

1. The first problem is just to implement and test a solution to the **fork/join problem**, as described in the text. Even though this solution is described in the text, the act of typing it in on your own is worthwhile; even Bach would rewrite Vivaldi, allowing one soon-to-be master to learn from an existing one. See `fork-join.c` for details. Add the call `sleep(1)` to the child to ensure it is working.

    macOS only supports named semaphore, so I use `sem_open()` here.

2. Let’s now generalize this a bit by investigating the **rendezvous problem**. The problem is as follows: you have two threads, each of which are about to enter the rendezvous point in the code. Neither should exit this part of the code before the other enters it. Consider using two semaphores for this task, and see `rendezvous.c` for details.

    *The Little Book of Semaphore* chapter 3.3

3. Now go one step further by implementing a general solution to **barrier synchronization**. Assume there are two points in a sequential piece of code, called *P<sub>1</sub>* and *P<sub>2</sub>*. Putting a barrier between *P<sub>1</sub>* and *P<sub>2</sub>* guarantees that all threads will execute *P<sub>1</sub>* before any one thread executes *P<sub>2</sub>*. Your task: write the code to implement a `barrier()` function that can be used in this manner. It is safe to assume you know *N* (the total number of threads in the running program) and that all *N* threads will try to enter the barrier. Again, you should likely use two semaphores to achieve the solution, and some other integers to count things. See `barrier.c` for details.

    *The Little Book of Semaphore* chapter 3.6

4. Now let’s solve the **reader-writer problem**, also as described in the text. In this first take, don’t worry about starvation. See the code in `reader-writer.c` for details. Add `sleep()` calls to your code to demonstrate it works as you expect. Can you show the existence of the starvation problem?

    >1. Any number of readers can be in the critical section simultaneously.
    >2. Writers must have exclusive access to the critical section.
    >
    >In other words, a writer cannot enter the critical section while any other thread (reader or writer) is there, and while the writer is there, no other thread may enter.
    >
    >--<cite>The Little Book of Semaphore chapter 4.2</cite>

5. Let’s look at the reader-writer problem again, but this time, worry about starvation. How can you ensure that all readers and writers eventually make progress? See `reader-writer-nostarve.c` for details.

6. Use semaphores to build a **no-starve mutex**, in which any thread that tries to acquire the mutex will eventually obtain it. See the code in `mutex-nostarve.c` for more information.

    *The Little Book of Semaphore* chapter 4.3

7. Liked these problems? See Downey’s free text for more just like them. And don’t forget, have fun! But, you always do when you write code, no?
