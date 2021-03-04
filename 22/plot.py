#! /usr/bin/env python3
# import os

# import subprocess
import matplotlib.pyplot as plt
import numpy as np

# cacheSizes = np.arange(0, 120, 20)
cacheSizes = np.arange(1, 5)
policies = ["FIFO", "LRU", "OPT", "UNOPT", "RAND", "CLOCK"]
hitRates = [
    [59.33, 84.85, 93.84, 96.00],
    [59.33, 88.48, 94.80, 96.94],
    [59.33, 89.43, 96.11, 97.69],
    [59.33, 59.33, 59.33, 59.33],
    [59.33, 84.87, 92.65, 95.24],
    [59.33, 85.69, 93.56, 95.90]
]

# for cacheSize in cacheSizes:
#     hitRate = []
#     for policy in policies:
#         result = subprocess.run(["./paging-policy.py", "-c", "-p", policy,
#             "-f", "./vpn.txt", "-C", str(cacheSize)], stdout=subprocess.PIPE)
#         result = result.stdout.decode('utf-8')
#         hitRate.append(result)
#     hitRates.append(hitRate)

for i in range(len(policies)):
    plt.plot(cacheSizes, hitRates[i])

plt.legend(policies)
plt.margins(0)
plt.xticks(cacheSizes, cacheSizes)
plt.xlabel('Cache Size (Blocks)')
plt.ylabel('Hit Rate')
plt.savefig('workload.png', dpi=227)
