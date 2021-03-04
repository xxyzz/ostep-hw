#! /usr/bin/env python3

import sys

import matplotlib.pyplot as plt
import numpy as np

matrix = np.loadtxt(sys.stdin)

plt.bar(matrix[:, 1], matrix[:, 0], color='orange', width=0.1)
plt.margins(0)
# plt.xticks(np.arange(10)/10, np.arange(10)/10)
# plt.tick_params(axis='x', which='major', labelsize=5)
plt.xlabel('Latency (milliseconds)')
plt.ylabel('Number')
plt.title('The latencies of request/reply pairs')
plt.savefig('latency.png', dpi=227)
plt.show()
