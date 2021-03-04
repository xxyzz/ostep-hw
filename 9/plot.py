#! /usr/bin/env python3

import random

import matplotlib.pyplot as plt
import numpy as np

uList = [0] * 1000

for i in range(30):
    random.seed(i)

    for jobLength in range(1, 1001):
        tickTotal = 200
        runTotal = 2 * jobLength
        jobs = 2
        quantum = 1
        jobList = [[0, jobLength, 100], [1, jobLength, 100]]

        clock = 0
        finishTime = []
        # run jobs
        for k in range(runTotal):
            r = int(random.random() * 1000001)
            winner = int(r % tickTotal)

            current = 0
            wjob = 0
            wrun = 0
            wtix = 0
            for (job, runtime, tickets) in jobList:
                current += tickets
                if current > winner and runtime > 0:
                    (wjob, wrun, wtix) = (job, runtime, tickets)
                    break

            # now do the accounting
            if wrun >= quantum:
                wrun -= quantum
            else:
                wrun = 0

            jobList[wjob] = (wjob, wrun, wtix)

            clock += quantum

            # job completed!
            if wrun == 0:
                tickTotal -= wtix
                jobs -= 1
                finishTime.append(clock)

            if jobs == 0:
                break

        u = round(finishTime[0] / finishTime[1], 2)
        uList[jobLength - 1] += u

fig = plt.figure()
x = np.linspace(1, 1000, 1000)
plt.plot(x, [u / 30 for u in uList], color='orange')
plt.ylim(0, 1)
plt.margins(0)
plt.xlabel('Job Length')
plt.ylabel('Unfairness (Average)')
plt.title('Figure 9.2: Lottery Fairness Study')
plt.savefig('9.2.png', dpi=227)
plt.show()
