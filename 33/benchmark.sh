#!/bin/bash

LOOP=0
if [ $# -ne 2 ]; then
    echo "Add test case and loop numbers then try again."
    echo "./benchmark 0 10: test async server with 10 clients"
    echo "./benchmark 1 10: test sync server with 10 clients"
    exit 1
else
    LOOP=$2
fi

if [ $1 -eq 0 ]; then
    echo "./TCPServer.out $LOOP &> ./async.txt &"
    $(./TCPServer.out $LOOP &> ./async.txt &)

    for (( i = 0; i < $LOOP; i++ ))
    do
        echo "./TCPClient.out 0 ./test.txt &> /dev/null &"
        $(./TCPClient.out 0 ./test.txt &> /dev/null &)
    done
fi

if [ $1 -eq 1 ]; then
    echo "./TCPServer-sync.out $LOOP &> ./sync.txt &"
    $(./TCPServer-sync.out $LOOP &> ./sync.txt &)

    for (( i = 0; i < $LOOP; i++ ))
    do
        echo "./TCPClient.out 0 ./test.txt &> /dev/null &"
        $(./TCPClient.out 0 ./test.txt &> /dev/null &)
    done
fi
