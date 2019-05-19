#!/bin/bash

FILENAME="./test.txt"
rm $FILENAME
TESTLOOP=64

function create_file {
    for i in $( seq $1 $2 )
    do
        echo -n 0 >> $FILENAME
    done

    echo "$(($2+1)) characters test file."
    ./check-xor.out      $FILENAME
    ./check-fletcher.out $FILENAME
    ./crc.out            $FILENAME
    echo ""
}

make
echo ""
for (( j = 1; j <= $TESTLOOP; j*=2 ))
do
    if [ $j -eq 1 ]; then
        create_file 0 $(($j*1000-1))
    else
        create_file $(($j*1000/2)) $(($j*1000-1))
    fi
done

rm $FILENAME
