#! /usr/bin/env python3
import os
import matplotlib.pyplot as plt
import numpy as np

x = np.arange(1, 8)
y = [6612.26, 6697.85, 6686.41, 3200.40, 1776.97, 1519.04, 1369.94]
fig = plt.figure()
plt.plot(x, y, marker='o', color='orange')
plt.margins(0)
plt.xlabel('Size of memory(1000 MB)')
plt.ylabel('Average bandwidth(MB/s)')
plt.savefig('bandwidth.png', dpi=227)
plt.clf()

x1 = np.arange(0, 11, 2)
y1 = [1680.19, 6032.19, 6795.72, 6763.85, 6808.33, 6484.74]

x2 = np.arange(11)
y2 = [1699.95, 6069.69, 6561.22, 6235.97, 6719.88, 6689.27, 6691.02, 6483.23, 6703.02, 6502.76, 6622.51]
y3 = [1696.53, 6127.96, 6696.09, 6681.92, 6605.30, 5935.54, 6736.97, 6714.58, 6664.68, 6706.24, 6298.27]
y4 = [1582.62, 1732.12, 2079.41, 2533.84, 3213.06, 3227.01, 3186.77, 3669.34, 2338.32, 2725.24, 5716.24]
y5 = [1569.29, 1660.40, 1817.28, 1558.05, 1798.59, 1435.46, 1412.03, 1640.72, 1719.02, 1453.76, 1705.09]
y6 = [1428.78, 1363.31, 1499.36, 1475.27, 1417.00, 1393.31, 1357.75, 1368.35, 1316.10, 1334.49, 1236.72]
y7 = [1386.50, 1259.12, 1324.51, 1193.17, 1245.85, 1239.99, 1265.39, 1256.14, 1101.80, 1083.07, 1343.84]

plt.plot(x1, y1)
plt.plot(x2, y2)
plt.plot(x2, y3)
plt.plot(x2, y4)
plt.plot(x2, y5)
plt.plot(x2, y6)
plt.plot(x2, y7)
plt.legend(['1024MB', '2048MB', '3072MB', '4096MB', '5120MB', '6144MB', '7168MB'])
plt.margins(0)
plt.xlabel('Loops')
plt.ylabel('Bandwidth(MB/s)')
plt.savefig('loops.png', dpi=227)
