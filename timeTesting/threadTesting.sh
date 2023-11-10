#!/bin/bash

startTime=$(date +%s%3N)

for threads in {0..8}
do
    echo -e "\e[38;2;255;200;0m\nSize $size\e[0m"

    startDisplay=$(date +"%T")
    echo -e "\e[38;2;255;200;0m$startDisplay running test ${shipCount[ship]}\e[0m"
    
    ./start.sh 7 5 $((2 ** $threads))
    
    endDisplay=$(date +"%T")
    echo -e "\e[38;2;255;100;0m$endDisplay test finished\e[0m\n"

done

endTime=$(date +%s%3N)
deltaTotalTime_ms=$(expr $endTime - $startTime)
deltaTotalTime=$(expr $deltaTotalTime_ms / 3600000)
echo "Tests Complete after $deltaTotalTime hrs ($deltaTotalTime_ms ms)"
