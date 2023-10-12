#!/bin/bash

# Compile the code
cd ./src/
rm *.class
javac *.java 
echo "Java Compiled"

# clear the file
echo -e -n "" > ../timeTesting/results_timeData_java.txt
echo -e -n "" > ../timeTesting/results_shipCount.txt


startTime=$(date +%s%3N)

threadCount=(1 4 4 4 4)
shipCount=("[2:2]" "[2:2,3:3]" "[2:2,3:3,3:a]" "[2:2,3:3,3:a,4:4]" "[2:2,3:3,3:a,4:4,5:5]")



for size in {3..10}
do
    echo -e "\e[38;2;255;200;0m\nSize $size\e[0m"

    echo -e -n "\n$size: " >> ../timeTesting/results_timeData_java.txt
    echo -e -n "\n$size: " >> ../timeTesting/results_shipCount.txt

    for ship in {0..4}
    do
        startDisplay=$(date +"%T")
        echo -e "\e[38;2;255;200;0m$startDisplay running test ${shipCount[ship]} (${threadCount[ship]} threads)\e[0m"
        
        fileNameDate=$(date +%Y%m%d-%H%M)
	java Runner $size "${shipCount[ship]}" ${threadCount[ship]} "$(printf '0%d' $size)-$ship" |& tee ../out/logs/$fileNameDate\_talita.log
        
        endDisplay=$(date +"%T")
        echo -e "\e[38;2;255;100;0m$endDisplay test finished\e[0m\n"


    done

    # Remove the last comma
    truncate -s -1 ../timeTesting/results_timeData_java.txt  
    truncate -s -1 ../timeTesting/results_shipCount.txt
done

endTime=$(date +%s%3N)
deltaTotalTime_ms=$(expr $endTime - $startTime)
deltaTotalTime=$(expr $deltaTotalTime_ms / 3600000)
echo "Tests Complete after $deltaTotalTime hrs ($deltaTotalTime_ms ms)"

cd ../timeTesting
python3 timePlots.py
