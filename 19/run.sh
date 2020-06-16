#!/usr/bin/env zsh

if [[ "$1" == "" ]]; then
    echo "Need max page number paramter"
    exit 1
fi

trails=1000
for (( i = 1; i <= $1; i *= 2 ))
do
    # echo "Number of pages: $i"
    ./tlb.out $i $trails $2
    wait
done
