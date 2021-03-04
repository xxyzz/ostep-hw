#! /usr/bin/env python3

import random

import matplotlib.pyplot as plt
import numpy as np

uList = [0] * 1000

for i in range(30):
    random.seed(i)

    for jobLength in range(1, 1001):
        runTotal = 2 * jobLength
        jobs = 2
        quantum = 1
        jobList = [[0, jobLength, 0], [1, jobLength, 0]]
        clock = 0
        finishTime = []
        r = int(random.random() * 1000001)
        stride = int(r % 100)
        # run jobs
        for k in range(runTotal):
            wjob = 0
            wrun = 0
            wpass = 0
            if (jobList[0][2] <= jobList[1][2]):
                # run job 0
                (wjob, wrun, wpass) = jobList[0]
            else:
                # run job 1
                (wjob, wrun, wpass) = jobList[1]

            # now do the accounting
            if wrun >= quantum:
                wrun -= quantum
            else:
                wrun = 0

            wpass += stride
            jobList[wjob] = (wjob, wrun, wpass)

            clock += quantum

            # job completed!
            if wrun == 0:
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
plt.title('Stride Scheduling')
plt.savefig('stride.png', dpi=227)
plt.show()
