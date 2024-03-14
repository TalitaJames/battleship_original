#!/bin/bash


startTime=$(date +%s%3N)

for size in {5..10}
do
    echo -e "\e[38;2;255;200;0m\nSize $size\e[0m"


    for ship in {1..5}
    do
        startDisplay=$(date +"%T")
        echo -e "\e[38;2;255;200;0m$startDisplay running test ${shipCount[ship]}\e[0m"
        
        ./start.sh $size $ship 8
        
        endDisplay=$(date +"%T")
        echo -e "\e[38;2;255;100;0m$endDisplay test finished\e[0m\n"


    done

    # Remove the last comma
done

endTime=$(date +%s%3N)
deltaTotalTime_ms=$(expr $endTime - $startTime)
deltaTotalTime=$(expr $deltaTotalTime_ms / 3600000)
echo "Tests Complete after $deltaTotalTime hrs ($deltaTotalTime_ms ms)"
