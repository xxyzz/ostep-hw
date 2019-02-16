#! /usr/bin/env python3
import os
import argparse
import subprocess
import matplotlib.pyplot as plt
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("trials", help="set iteration number")
args = parser.parse_args()

if not os.path.isfile("./tlb.out"):
    subprocess.run(["gcc", "-o", "tlb.out", "tlb.c", "-Wall"])

y = []
a = np.arange(11)
x = 2**a
for i in x:
    result = subprocess.run(["./tlb.out", str(i), args.trials], stdout=subprocess.PIPE)
    result = result.stdout.decode('utf-8')
    y.append(float(result))

fig = plt.figure()
plt.plot(a, y, marker='o', color='orange')
plt.margins(0)
plt.xticks(a, x) # evenly spaced
plt.xlabel('Number Of Pages')
plt.ylabel('Time Per Access (ns)')
plt.title('TLB Size Measurement')
plt.savefig('tlb.png')
plt.show()
