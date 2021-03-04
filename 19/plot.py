#!/usr/bin/env python3

import sys

import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt(sys.stdin)
pages = len(data)
a = np.arange(pages)
x = 2**a

plt.plot(a, data[:, 0], marker='o', color='orange')
plt.plot(a, data[:, 1], marker='o')
plt.plot(a, data[:, 2], marker='o')
plt.margins(0)
plt.xticks(a, x, rotation=-20, fontsize='x-small')  # evenly spaced
plt.xlabel('Number Of Pages')
plt.ylabel('Time Per Access (ns)')
title = 'TLB Size Measurement(single CPU)'
file_name = 'tlb_single.png'
if len(sys.argv) > 1:
    title = 'TLB Size Measurement(multiple CPU)'
    file_name = 'tlb_multiple.png'
plt.title(title)
plt.legend(['real', 'user', 'sys'])
plt.savefig(file_name, dpi=227)
plt.show()
