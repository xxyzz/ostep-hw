#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

# traditional = [0.019966, 0.095837, 0.180244, 0.195340]
# approximate = [0.021546, 0.108778, 0.191885, 0.248192]
approximate = [0.430974, 0.396419, 0.309085, 0.273745, 0.266137,
               0.258349, 0.249036, 0.252860, 0.242337, 0.242711, 0.251097]
a = np.arange(0, 11)
s = 2 ** a

# plt.plot(a, traditional, marker='x')
plt.plot(a, approximate, marker='x')
plt.margins(0)
plt.xticks(a, s)
plt.xlabel('Approximation Factor (S)')
plt.ylabel('Time (seconds)')
# plt.legend(['Precise', 'Approximate'])
plt.savefig('approximate.png', dpi=300)
plt.show()
