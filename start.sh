#!/bin/bash
ships=("{1}" "{2}" "{2,3}" "{2,3,3}" "{2,3,3,4}" "{2,3,3,4,5}")

if [ "$#" = 2 ]; then
    boardSize=$1
    shipSize=$2
    threadCount=8
fi

if [ "$#" = 3 ]; then
    boardSize=$1
    shipSize=$2
    threadCount=$3
fi

if [ "$#" -ne 2 ] && [ "$#" -ne 3 ]; then
    boardSize=8
    shipSize=3
    threadCount=8
fi



echo "$boardSize with $shipSize ships ${ships[$shipSize]} with $threadCount threads"
cd src

# Change the header file to the new input args
sed -r -i  "s/^const int BOARD_SIZE = .*;/const int BOARD_SIZE = $boardSize;/" runner.h
sed -r -i  "s/^const ship FLEET\[\] =.*;/const ship FLEET[] = ${ships[$shipSize]};/" runner.h
sed -r -i  "s/^int threadCount = .*;/int threadCount = $threadCount;/" runner.h


rm runner.out
make
echo "cpp ready"
fileNameDate=$(date +%Y%m%d-%H%M)
time ./runner.out |& tee ../out/logs/$fileNameDate\_talita-cpp.log

python3 plotTurns.py

