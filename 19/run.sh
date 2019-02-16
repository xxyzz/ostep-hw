#!/bin/bash
if [ "$1" == "" ]; then
    echo "Need iteration paramter"
    exit 1
fi
if [ ! -f ./tlb.out ]; then
    gcc -o tlb.out tlb.c -Wall
fi
for (( i = 1; i < 1000; i *= 2 ))
do
    ./tlb.out $i $1
done
