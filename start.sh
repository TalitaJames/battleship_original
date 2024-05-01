#!/bin/bash
ships=("{1}" "{2}" "{2,3}" "{2,3,3}" "{2,3,3,4}" "{2,3,3,4,5}")

if [ "$#" = 2 ]; then
    boardSize=$1
    shipSize=$2
    threadCount=8
elif [ "$#" = 3 ]; then
    boardSize=$1
    shipSize=$2
    threadCount=$3
elif [ "$#" -ne 2 ] && [ "$#" -ne 3 ]; then
    boardSize=5
    shipSize=3
    threadCount=8
fi

echo "$boardSize with $shipSize ships ${ships[$shipSize]} with $threadCount threads"
cd src

# Change the header file to the new input args
sed -r -i  "s/^const int BOARD_SIZE = .*;/const int BOARD_SIZE = $boardSize;/" boatsAndBoards.h
sed -r -i  "s/^const ship FLEET\[\] =.*;/const ship FLEET[] = ${ships[$shipSize]};/" boatsAndBoards.h
sed -r -i  "s/^int threadCount = .*;/int threadCount = $threadCount;/" boatsAndBoards.cpp

rm runner.out
make
fileNameDate=$(date +%Y%m%d-%H%M%S)
time ./runner.out |& tee ../out/logs/$fileNameDate\_talita.log


# valgrind --leak-check=full \
#          --log-file=../out/logs/$fileNameDate\_valgrind.log \
#          ./runner.out
#         #  --show-leak-kinds=all \
#         #  --track-origins=yes \
#         #  --verbose \

cd ..
# time python3 src/plot/heatmap.py
