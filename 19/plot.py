#!/usr/bin/env python3

import argparse
import platform
import shutil
import subprocess

import matplotlib.pyplot as plt
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument('pages', type=int, help='input 4, 1 to 2^3 pages')
parser.add_argument('trials')
parser.add_argument('--single_cpu', action='store_true')
args = parser.parse_args()

data = []
x = np.arange(args.pages)
pages = 2 ** x
for i in pages:
    if args.single_cpu:
        if shutil.which('hwloc-bind'):
            r = subprocess.run(
                ['hwloc-bind', '--single', './tlb.out', str(i), args.trials],
                capture_output=True, check=True, text=True)
        elif shutil.which('taskset'):
            r = subprocess.run(
                ['taskset', '-c', '0', './tlb.out', str(i), args.trials],
                capture_output=True, check=True, text=True)
        else:
            raise Exception("Can't find hwloc-bind or taskset")
    else:
        r = subprocess.run(['./tlb.out', str(i), args.trials],
                           capture_output=True, check=True, text=True)
    data.append(float(r.stdout))

plt.plot(x, data, marker='o', color='orange')
plt.margins(0)
plt.xticks(x, pages, fontsize='x-small')  # evenly spaced
plt.xlabel('Number Of Pages')
plt.ylabel('Time Per Access (ns)')
title = 'TLB Size Measurement: '
title += f'{platform.system()} {platform.release()} {platform.machine()}'
if args.single_cpu:
    title += ' single CPU'
plt.title(title)
plt.savefig(
    f'{platform.system()}{"_single.png" if args.single_cpu else ".png"}',
    dpi=227)
plt.show()  # pacman -S tk
