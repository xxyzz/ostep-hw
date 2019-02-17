#!/bin/bash
if [ "$1" == "" ]; then
    echo "Need iteration paramter"
    exit 1
fi
if [ ! -f ./tlb.out ]; then
    gcc -O0 -o tlb.out tlb.c -Wall
fi
for (( i = 1; i <= 8192; i *= 2 ))
do
    ./tlb.out $i $1
done
