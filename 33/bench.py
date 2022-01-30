#!/usr/bin/env python3

import argparse
import platform
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("file_name")
parser.add_argument("num_reqs", type=int)
parser.add_argument("trials", type=int)
args = parser.parse_args()


def bench(lib):
    total_time = 0
    for _ in range(args.trials):
        sp = subprocess.Popen(
            [f'./server_{lib}.out', str(args.num_reqs)],
            stderr=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        cp = subprocess.Popen(
            ['./client.out', args.file_name, str(args.num_reqs)],
            stderr=subprocess.PIPE, stdout=subprocess.DEVNULL, text=True)
        _, c_err = cp.communicate()
        if c_err:
            print(f'client error: {c_err}')
            sp.terminate()
            exit(1)
        s_out, s_err = sp.communicate()
        if s_err:
            print(f'server error: {s_err}')
            exit(1)
        total_time += float(s_out)

    print(f'{lib}: {total_time / (args.trials * args.num_reqs)} '
          'nanoseconds/request')


if platform.system() == 'Linux':
    bench('io_uring')  # async
    bench('epoll')     # sync
bench('libevent')      # nonblock
