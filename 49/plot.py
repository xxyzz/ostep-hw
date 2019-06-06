#! /usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import sys

matrix = np.loadtxt(sys.stdin)

print(matrix[-5:, :])

plt.bar(matrix[:, 1], matrix[:, 0], color='orange')
plt.margins(0)
plt.xlabel('Latency (seconds)')
plt.ylabel('Number')
plt.title('The latencies of request/reply pairs')
plt.savefig('latency.png', dpi=227)
plt.show()
